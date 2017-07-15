/* A handle to something
(C) 2015-2017 Niall Douglas <http://www.nedproductions.biz/> (20 commits)
File Created: Dec 2015


Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License in the accompanying file
Licence.txt or at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.


Distributed under the Boost Software License, Version 1.0.
    (See accompanying file Licence.txt or copy at
          http://www.boost.org/LICENSE_1_0.txt)
*/

#ifndef AFIO_HANDLE_H
#define AFIO_HANDLE_H

#include "deadline.h"
#include "native_handle_type.hpp"

#include "../quickcpplib/include/uint128.hpp"

#include <algorithm>  // for std::count
#include <utility>    // for pair<>

//! \file handle.hpp Provides handle

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4251)  // dll interface
#endif

AFIO_V2_NAMESPACE_EXPORT_BEGIN

/*! \class handle
\brief A native_handle_type which is managed by the lifetime of this object instance.
*/
class AFIO_DECL handle
{
  friend inline std::ostream &operator<<(std::ostream &s, const handle &v);

public:
  //! The path type used by this handle
  using path_type = fixme_path;
  //! The file extent type used by this handle
  using extent_type = unsigned long long;
  //! The memory extent type used by this handle
  using size_type = size_t;
  //! The unique identifier type used by this handle
  using unique_id_type = QUICKCPPLIB_NAMESPACE::integers128::uint128;

  //! The behaviour of the handle: does it read, read and write, or atomic append?
  enum class mode : unsigned char  // bit 0 set means writable
  {
    unchanged = 0,
    none = 2,        //!< No ability to read or write anything, but can synchronise (SYNCHRONIZE or 0)
    attr_read = 4,   //!< Ability to read attributes (FILE_READ_ATTRIBUTES|SYNCHRONIZE or O_RDONLY)
    attr_write = 5,  //!< Ability to read and write attributes (FILE_READ_ATTRIBUTES|FILE_WRITE_ATTRIBUTES|SYNCHRONIZE or O_RDONLY)
    read = 6,        //!< Ability to read (READ_CONTROL|FILE_READ_DATA|FILE_READ_ATTRIBUTES|FILE_READ_EA|SYNCHRONISE or O_RDONLY)
    write = 7,       //!< Ability to read and write (READ_CONTROL|FILE_READ_DATA|FILE_READ_ATTRIBUTES|FILE_READ_EA|FILE_WRITE_DATA|FILE_WRITE_ATTRIBUTES|FILE_WRITE_EA|FILE_APPEND_DATA|SYNCHRONISE or O_RDWR)
    append = 9       //!< All mainstream OSs and CIFS guarantee this is atomic with respect to all other appenders (FILE_APPEND_DATA|SYNCHRONISE or O_APPEND)
                     // NOTE: IF UPDATING THIS UPDATE THE std::ostream PRINTER BELOW!!!
  };
  //! On opening, do we also create a new file or truncate an existing one?
  enum class creation : unsigned char
  {
    open_existing = 0,
    only_if_not_exist,
    if_needed,
    truncate  //!< Atomically truncate on open, leaving creation date unmodified.
              // NOTE: IF UPDATING THIS UPDATE THE std::ostream PRINTER BELOW!!!
  };
  //! What i/o on the handle will complete immediately due to kernel caching
  enum class caching : unsigned char  // bit 0 set means safety fsyncs enabled
  {
    unchanged = 0,
    none = 1,                //!< No caching whatsoever, all reads and writes come from storage (i.e. <tt>O_DIRECT|O_SYNC</tt>). Align all i/o to 4Kb boundaries for this to work. <tt>flag_disable_safety_fsyncs</tt> can be used here.
    only_metadata = 2,       //!< Cache reads and writes of metadata but avoid caching data (<tt>O_DIRECT</tt>), thus i/o here does not affect other cached data for other handles. Align all i/o to 4Kb boundaries for this to work.
    reads = 3,               //!< Cache reads only. Writes of data and metadata do not complete until reaching storage (<tt>O_SYNC</tt>). <tt>flag_disable_safety_fsyncs</tt> can be used here.
    reads_and_metadata = 5,  //!< Cache reads and writes of metadata, but writes of data do not complete until reaching storage (<tt>O_DSYNC</tt>). <tt>flag_disable_safety_fsyncs</tt> can be used here.
    all = 4,                 //!< Cache reads and writes of data and metadata so they complete immediately, sending writes to storage at some point when the kernel decides (this is the default file system caching on a system).
    safety_fsyncs = 7,       //!< Cache reads and writes of data and metadata so they complete immediately, but issue safety fsyncs at certain points. See documentation for <tt>flag_disable_safety_fsyncs</tt>.
    temporary = 6            //!< Cache reads and writes of data and metadata so they complete immediately, only sending any updates to storage on last handle close in the system or if memory becomes tight as this file is expected to be temporary (Windows and FreeBSD only).
                             // NOTE: IF UPDATING THIS UPDATE THE std::ostream PRINTER BELOW!!!
  };
  //! Bitwise flags which can be specified
  QUICKCPPLIB_BITFIELD_BEGIN(flag)
  {
    none = 0,  //!< No flags
    /*! Unlinks the file on handle close. On POSIX, this simply unlinks whatever is pointed
    to by `path()` upon the call of `close()` if and only if the inode matches. On Windows,
    this opens the file handle with the `FILE_FLAG_DELETE_ON_CLOSE` modifier which substantially
    affects caching policy and causes the \b first handle close to make the file unavailable for
    anyone else to open with an `errc::resource_unavailable_try_again` error return. Because this is confusing, unless the
    `win_disable_unlink_emulation` flag is also specified, this POSIX behaviour is
    somewhat emulated by AFIO on Windows by renaming the file to a random name on `close()`
    causing it to appear to have been unlinked immediately.
    */
    unlink_on_close = 1 << 0,

    /*! Some kernel caching modes have unhelpfully inconsistent behaviours
    in getting your data onto storage, so by default unless this flag is
    specified AFIO adds extra fsyncs to the following operations for the
    caching modes specified below:
    * truncation of file length either explicitly or during file open.
    * closing of the handle either explicitly or in the destructor.

    Additionally on Linux only to prevent loss of file metadata:
    * On the parent directory whenever a file might have been created.
    * On the parent directory on file close.

    This only occurs for these kernel caching modes:
    * caching::none
    * caching::reads
    * caching::reads_and_metadata
    * caching::safety_fsyncs
    */
    disable_safety_fsyncs = 1 << 2,
    /*! `file_handle::unlink()` could accidentally delete the wrong file if someone has
    renamed the open file handle since the time it was opened. To prevent this occuring,
    where the OS doesn't provide race free unlink-by-open-handle we compare the inode of
    the path we are about to unlink with that of the open handle before unlinking.
    \warning This does not prevent races where in between the time of checking the inode
    and executing the unlink a third party changes the item about to be unlinked. Only
    operating systems with a true race-free unlink syscall are race free.
    */
    disable_safety_unlinks = 1 << 3,

    win_disable_unlink_emulation = 1 << 24,  //!< See the documentation for `unlink_on_close`

    // NOTE: IF UPDATING THIS UPDATE THE std::ostream PRINTER BELOW!!!

    overlapped = 1 << 28,         //!< On Windows, create any new handles with OVERLAPPED semantics
    byte_lock_insanity = 1 << 29  //!< Using insane POSIX byte range locks
  }
  QUICKCPPLIB_BITFIELD_END(flag)
protected:
  caching _caching;
  flag _flags;
  native_handle_type _v;

public:
  //! Default constructor
  constexpr handle()
      : _caching(caching::none)
      , _flags(flag::none)
  {
  }
  //! Construct a handle from a supplied native handle
  QUICKCPPLIB_CONSTEXPR handle(native_handle_type h, caching caching = caching::none, flag flags = flag::none)
      : _caching(caching)
      , _flags(flags)
      , _v(std::move(h))
  {
  }
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC ~handle();
  //! No copy construction (use clone())
  handle(const handle &) = delete;
  //! No copy assignment
  handle &operator=(const handle &o) = delete;
  //! Move the handle.
  handle(handle &&o) noexcept : _caching(o._caching), _flags(o._flags), _v(std::move(o._v))
  {
    o._caching = caching::none;
    o._flags = flag::none;
    o._v = native_handle_type();
  }
  //! Move assignment of handle
  handle &operator=(handle &&o) noexcept
  {
    this->~handle();
    new(this) handle(std::move(o));
    return *this;
  }
  //! Swap with another instance
  void swap(handle &o) noexcept
  {
    handle temp(std::move(*this));
    *this = std::move(o);
    o = std::move(temp);
  }

  //! A unique identifier for this handle in this process (native handle). Subclasses like `file_handle` make this a unique identifier across the entire system.
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC unique_id_type unique_id() const noexcept
  {
    unique_id_type ret(nullptr);
    ret.as_longlongs[0] = _v._init;
    return ret;
  }
  //! The path this handle refers to, if any
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC path_type path() const noexcept { return path_type(); }
  //! Immediately close the native handle type managed by this handle
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC result<void> close() noexcept;
  /*! Clone this handle (copy constructor is disabled to avoid accidental copying)

  \errors Any of the values POSIX dup() or DuplicateHandle() can return.
  */
  AFIO_HEADERS_ONLY_MEMFUNC_SPEC result<handle> clone() const noexcept;
  //! Release the native handle type managed by this handle
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC native_handle_type release() noexcept
  {
    native_handle_type ret(std::move(_v));
    return ret;
  }

  //! True if the handle is valid (and usually open)
  bool is_valid() const noexcept { return _v.is_valid(); }

  //! True if the handle is readable
  bool is_readable() const noexcept { return _v.is_readable(); }
  //! True if the handle is writable
  bool is_writable() const noexcept { return _v.is_writable(); }
  //! True if the handle is append only
  bool is_append_only() const noexcept { return _v.is_append_only(); }
  /*! Changes whether this handle is append only or not.

  \warning On Windows this is implemented as a bit of a hack to make it fast like on POSIX,
  so make sure you open the handle for read/write originally. Note unlike on POSIX the
  append_only disposition will be the only one toggled, seekable and readable will remain
  turned on.

  \errors Whatever POSIX fcntl() returns. On Windows nothing is changed on the handle.
  \mallocs No memory allocation.
  */
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC result<void> set_append_only(bool enable) noexcept;

  //! True if overlapped
  bool is_overlapped() const noexcept { return _v.is_overlapped(); }
  //! True if seekable
  bool is_seekable() const noexcept { return _v.is_seekable(); }
  //! True if requires aligned i/o
  bool requires_aligned_io() const noexcept { return _v.requires_aligned_io(); }

  //! True if a regular file or device
  bool is_regular() const noexcept { return _v.is_regular(); }
  //! True if a directory
  bool is_directory() const noexcept { return _v.is_directory(); }
  //! True if a symlink
  bool is_symlink() const noexcept { return _v.is_symlink(); }
  //! True if a multiplexer like BSD kqueues, Linux epoll or Windows IOCP
  bool is_multiplexer() const noexcept { return _v.is_multiplexer(); }
  //! True if a process
  bool is_process() const noexcept { return _v.is_process(); }
  //! True if a memory section
  bool is_section() const noexcept { return _v.is_section(); }

  //! Kernel cache strategy used by this handle
  caching kernel_caching() const noexcept { return _caching; }
  //! True if the handle uses the kernel page cache for reads
  bool are_reads_from_cache() const noexcept { return _caching != caching::none && _caching != caching::only_metadata; }
  //! True if writes are safely on storage on completion
  bool are_writes_durable() const noexcept { return _caching == caching::none || _caching == caching::reads || _caching == caching::reads_and_metadata; }
  //! True if issuing safety fsyncs is on
  bool are_safety_fsyncs_issued() const noexcept { return !(_flags & flag::disable_safety_fsyncs) && !!(static_cast<int>(_caching) & 1); }
  /*! Changes the kernel cache strategy used by this handle.
  Note most OSs impose severe restrictions on what can be changed and will error out,
  it may be easier to simply create a new handle.

  \warning On Windows this reopens the file, it is no slower than
  opening the file fresh but equally it is vastly slower than on POSIX.

  \errors Whatever POSIX fcntl() or ReOpenFile() returns.
  \mallocs No memory allocation.
  */
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC result<void> set_kernel_caching(caching caching) noexcept;

  //! The flags this handle was opened with
  flag flags() const noexcept { return _flags; }
  //! The native handle used by this handle
  native_handle_type native_handle() const noexcept { return _v; }
};
inline std::ostream &operator<<(std::ostream &s, const handle &v)
{
  return s << "afio::handle(" << v._v._init << ", " << v.path() << ")";
}
inline std::ostream &operator<<(std::ostream &s, const handle::mode &v)
{
  static constexpr const char *values[] = {"unchanged", nullptr, "none", nullptr, "attr_read", "attr_write", "read", "write", nullptr, "append"};
  if(static_cast<size_t>(v) >= sizeof(values) / sizeof(values[0]) || !values[static_cast<size_t>(v)])
    return s << "afio::handle::mode::<unknown>";
  return s << "afio::handle::mode::" << values[static_cast<size_t>(v)];
}
inline std::ostream &operator<<(std::ostream &s, const handle::creation &v)
{
  static constexpr const char *values[] = {"open_existing", "only_if_not_exist", "if_needed", "truncate"};
  if(static_cast<size_t>(v) >= sizeof(values) / sizeof(values[0]) || !values[static_cast<size_t>(v)])
    return s << "afio::handle::creation::<unknown>";
  return s << "afio::handle::creation::" << values[static_cast<size_t>(v)];
}
inline std::ostream &operator<<(std::ostream &s, const handle::caching &v)
{
  static constexpr const char *values[] = {"unchanged", "none", "only_metadata", "reads", "all", "reads_and_metadata", "temporary", "safety_fsyncs"};
  if(static_cast<size_t>(v) >= sizeof(values) / sizeof(values[0]) || !values[static_cast<size_t>(v)])
    return s << "afio::handle::caching::<unknown>";
  return s << "afio::handle::caching::" << values[static_cast<size_t>(v)];
}
inline std::ostream &operator<<(std::ostream &s, const handle::flag &v)
{
  std::string temp;
  if(!!(v & handle::flag::unlink_on_close))
    temp.append("unlink_on_close|");
  if(!!(v & handle::flag::disable_safety_fsyncs))
    temp.append("disable_safety_fsyncs|");
  if(!!(v & handle::flag::win_disable_unlink_emulation))
    temp.append("win_disable_unlink_emulation|");
  if(!!(v & handle::flag::overlapped))
    temp.append("overlapped|");
  if(!!(v & handle::flag::byte_lock_insanity))
    temp.append("byte_lock_insanity|");
  if(!temp.empty())
  {
    temp.resize(temp.size() - 1);
    if(std::count(temp.cbegin(), temp.cend(), '|') > 0)
      temp = "(" + temp + ")";
  }
  else
    temp = "none";
  return s << "afio::handle::flag::" << temp;
}

/*! \class io_handle
\brief A handle to something capable of scatter-gather i/o.
*/
class AFIO_DECL io_handle : public handle
{
public:
  using path_type = handle::path_type;
  using extent_type = handle::extent_type;
  using size_type = handle::size_type;
  using mode = handle::mode;
  using creation = handle::creation;
  using caching = handle::caching;
  using flag = handle::flag;

  //! The scatter buffer type used by this handle
  using buffer_type = std::pair<char *, size_type>;
  //! The gather buffer type used by this handle
  using const_buffer_type = std::pair<const char *, size_type>;
  //! The scatter buffers type used by this handle
  using buffers_type = span<buffer_type>;
  //! The gather buffers type used by this handle
  using const_buffers_type = span<const_buffer_type>;
  //! The i/o request type used by this handle
  template <class T> struct io_request
  {
    T buffers;
    extent_type offset;
    constexpr io_request()
        : buffers()
        , offset(0)
    {
    }
    constexpr io_request(T _buffers, extent_type _offset)
        : buffers(std::move(_buffers))
        , offset(_offset)
    {
    }
  };
  //! The i/o result type used by this handle
  template <class T> class io_result : public result<T>
  {
    using Base = result<T>;
    size_type _bytes_transferred;

  public:
    constexpr io_result() noexcept : _bytes_transferred((size_type) -1) {}
    template <class... Args>
    io_result(Args &&... args)
        : result<T>(std::forward<Args>(args)...)
        , _bytes_transferred((size_type) -1)
    {
    }
    io_result &operator=(const io_result &) = default;
    io_result &operator=(io_result &&) = default;
    //! Returns bytes transferred
    size_type bytes_transferred() noexcept
    {
      if(_bytes_transferred == (size_type) -1)
      {
        _bytes_transferred = 0;
        for(auto &i : this->value())
          _bytes_transferred += i.second;
      }
      return _bytes_transferred;
    }
  };

public:
  //! Default constructor
  constexpr io_handle() = default;
  //! Construct a handle from a supplied native handle
  QUICKCPPLIB_CONSTEXPR io_handle(native_handle_type h, caching caching = caching::none, flag flags = flag::none)
      : handle(h, caching, flags)
  {
  }
  //! Explicit conversion from handle permitted
  explicit io_handle(handle &&o) noexcept : handle(std::move(o)) {}
  //! Move construction permitted
  io_handle(io_handle &&) = default;
  //! Move assignment permitted
  io_handle &operator=(io_handle &&) = default;

  /*! \brief Read data from the open handle.

  \return The buffers read, which may not be the buffers input. The size of each scatter-gather
  buffer is updated with the number of bytes of that buffer transferred, and the pointer to
  the data may be \em completely different to what was submitted (e.g. it may point into a
  memory map).
  \param reqs A scatter-gather and offset request.
  \param d An optional deadline by which the i/o must complete, else it is cancelled.
  Note function may return significantly after this deadline if the i/o takes long to cancel.
  \errors Any of the values POSIX read() can return, `errc::timed_out`, `errc::operation_canceled`. `errc::not_supported` may be
  returned if deadline i/o is not possible with this particular handle configuration (e.g.
  reading from regular files on POSIX or reading from a non-overlapped HANDLE on Windows).
  \mallocs The default synchronous implementation in file_handle performs no memory allocation.
  The asynchronous implementation in async_file_handle performs one calloc and one free.
  */
  //[[bindlib::make_free]]
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC io_result<buffers_type> read(io_request<buffers_type> reqs, deadline d = deadline()) noexcept;
  //! \overload
  io_result<buffer_type> read(extent_type offset, char *data, size_type bytes, deadline d = deadline()) noexcept
  {
    buffer_type _reqs[1] = {{data, bytes}};
    io_request<buffers_type> reqs(buffers_type(_reqs), offset);
    OUTCOME_TRY(v, read(reqs, d));
    return *v.data();
  }

  /*! \brief Write data to the open handle.

  \return The buffers written, which may not be the buffers input. The size of each scatter-gather
  buffer is updated with the number of bytes of that buffer transferred.
  \param reqs A scatter-gather and offset request.
  \param d An optional deadline by which the i/o must complete, else it is cancelled.
  Note function may return significantly after this deadline if the i/o takes long to cancel.
  \errors Any of the values POSIX write() can return, `errc::timed_out`, `errc::operation_canceled`. `errc::not_supported` may be
  returned if deadline i/o is not possible with this particular handle configuration (e.g.
  writing to regular files on POSIX or writing to a non-overlapped HANDLE on Windows).
  \mallocs The default synchronous implementation in file_handle performs no memory allocation.
  The asynchronous implementation in async_file_handle performs one calloc and one free.
  */
  //[[bindlib::make_free]]
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC io_result<const_buffers_type> write(io_request<const_buffers_type> reqs, deadline d = deadline()) noexcept;
  //! \overload
  io_result<const_buffer_type> write(extent_type offset, const char *data, size_type bytes, deadline d = deadline()) noexcept
  {
    const_buffer_type _reqs[1] = {{data, bytes}};
    io_request<const_buffers_type> reqs(const_buffers_type(_reqs), offset);
    OUTCOME_TRY(v, write(reqs, d));
    return *v.data();
  }

  /*! \brief Issue a write reordering barrier such that writes preceding the barrier will reach storage
  before writes after this barrier.

  \warning **Assume that this call is a no-op**. It is not reliably implemented in many common use cases,
  for example if your code is running inside a LXC container, or if the user has mounted the filing
  system with non-default options. Instead open the handle with `caching::reads` which means that all
  writes form a strict sequential order not completing until acknowledged by the storage device.
  Filing system can and do use different algorithms to give much better performance with `caching::reads`,
  some (e.g. ZFS) spectacularly better.

  \warning Let me repeat again: consider this call to be a **hint** to poke the kernel with a stick to
  go start to do some work sooner rather than later. It may be ignored entirely.

  \warning For portability, you can only assume that barriers write order for a single handle
  instance. You cannot assume that barriers write order across multiple handles to the same inode, or
  across processes.

  \return The buffers barriered, which may not be the buffers input. The size of each scatter-gather
  buffer is updated with the number of bytes of that buffer barriered.
  \param reqs A scatter-gather and offset request for what range to barrier. May be ignored on some platforms
  which always write barrier the entire file. Supplying a default initialised reqs write barriers the entire file.
  \param wait_for_device True if you want the call to wait until data reaches storage and that storage
  has acknowledged the data is physically written. Slow.
  \param and_metadata True if you want the call to sync the metadata for retrieving the writes before the
  barrier after a sudden power loss event. Slow.
  \param d An optional deadline by which the i/o must complete, else it is cancelled.
  Note function may return significantly after this deadline if the i/o takes long to cancel.
  \errors Any of the values POSIX fdatasync() or Windows NtFlushBuffersFileEx() can return.
  \mallocs None.
  */
  //[[bindlib::make_free]]
  virtual io_result<const_buffers_type> barrier(io_request<const_buffers_type> reqs = io_request<const_buffers_type>(), bool wait_for_device = false, bool and_metadata = false, deadline d = deadline()) noexcept = 0;

  /*! \class extent_guard
  \brief RAII holder a locked extent of bytes in a file.
  */
  class extent_guard
  {
    friend class io_handle;
    io_handle *_h;
    extent_type _offset, _length;
    bool _exclusive;
    constexpr extent_guard(io_handle *h, extent_type offset, extent_type length, bool exclusive)
        : _h(h)
        , _offset(offset)
        , _length(length)
        , _exclusive(exclusive)
    {
    }
    extent_guard(const extent_guard &) = delete;
    extent_guard &operator=(const extent_guard &) = delete;

  public:
    //! Default constructor
    constexpr extent_guard()
        : _h(nullptr)
        , _offset(0)
        , _length(0)
        , _exclusive(false)
    {
    }
    //! Move constructor
    extent_guard(extent_guard &&o) noexcept : _h(o._h), _offset(o._offset), _length(o._length), _exclusive(o._exclusive) { o.release(); }
    //! Move assign
    extent_guard &operator=(extent_guard &&o) noexcept
    {
      unlock();
      _h = o._h;
      _offset = o._offset;
      _length = o._length;
      _exclusive = o._exclusive;
      o.release();
      return *this;
    }
    ~extent_guard()
    {
      if(_h)
        unlock();
    }
    //! True if extent guard is valid
    explicit operator bool() const noexcept { return _h != nullptr; }
    //! True if extent guard is invalid
    bool operator!() const noexcept { return _h == nullptr; }

    //! The io_handle to be unlocked
    io_handle *handle() const noexcept { return _h; }
    //! Sets the io_handle to be unlocked
    void set_handle(io_handle *h) noexcept { _h = h; }
    //! The extent to be unlocked
    std::tuple<extent_type, extent_type, bool> extent() const noexcept { return std::make_tuple(_offset, _length, _exclusive); }

    //! Unlocks the locked extent immediately
    void unlock() noexcept
    {
      if(_h)
      {
        _h->unlock(_offset, _length);
        release();
      }
    }

    //! Detach this RAII unlocker from the locked state
    void release() noexcept
    {
      _h = nullptr;
      _offset = 0;
      _length = 0;
      _exclusive = false;
    }
  };

  /*! \brief Tries to lock the range of bytes specified for shared or exclusive access. Be aware this passes through
  the same semantics as the underlying OS call, including any POSIX insanity present on your platform.

  \warning On older Linuxes and POSIX, this uses `fcntl()` with the well known insane POSIX
  semantics that closing ANY handle to this file releases all bytes range locks on it. If your
  OS isn't new enough to support the non-insane lock API, `flag::byte_lock_insanity` will be set
  in flags() after the first call to this function.

  \return An extent guard, the destruction of which will call unlock().
  \param offset The offset to lock. Note that on POSIX the top bit is always cleared before use
  as POSIX uses signed transport for offsets. If you want an advisory rather than mandatory lock
  on Windows, one technique is to force top bit set so the region you lock is not the one you will
  i/o - obviously this reduces maximum file size to (2^63)-1.
  \param bytes The number of bytes to lock. Zero means lock the entire file using any more
  efficient alternative algorithm where available on your platform (specifically, on BSD and OS X use
  flock() for non-insane semantics).
  \param exclusive Whether the lock is to be exclusive.
  \param d An optional deadline by which the lock must complete, else it is cancelled.
  \errors Any of the values POSIX fcntl() can return, `errc::timed_out`, `errc::not_supported` may be
  returned if deadline i/o is not possible with this particular handle configuration (e.g.
  non-overlapped HANDLE on Windows).
  \mallocs The default synchronous implementation in file_handle performs no memory allocation.
  The asynchronous implementation in async_file_handle performs one calloc and one free.
  */
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC result<extent_guard> lock(extent_type offset, extent_type bytes, bool exclusive = true, deadline d = deadline()) noexcept;
  //! \overload
  result<extent_guard> try_lock(extent_type offset, extent_type bytes, bool exclusive = true) noexcept { return lock(offset, bytes, exclusive, deadline(std::chrono::seconds(0))); }
  //! \overload Locks for shared access
  result<extent_guard> lock(io_request<buffers_type> reqs, deadline d = deadline()) noexcept
  {
    size_t bytes = 0;
    for(auto &i : reqs.buffers)
      bytes += i.second;
    return lock(reqs.offset, bytes, false, std::move(d));
  }
  //! \overload Locks for exclusive access
  result<extent_guard> lock(io_request<const_buffers_type> reqs, deadline d = deadline()) noexcept
  {
    size_t bytes = 0;
    for(auto &i : reqs.buffers)
      bytes += i.second;
    return lock(reqs.offset, bytes, true, std::move(d));
  }

  /*! \brief Unlocks a byte range previously locked.

  \param offset The offset to unlock. This should be an offset previously locked.
  \param bytes The number of bytes to unlock. This should be a byte extent previously locked.
  \errors Any of the values POSIX fcntl() can return.
  \mallocs None.
  */
  AFIO_HEADERS_ONLY_VIRTUAL_SPEC void unlock(extent_type offset, extent_type bytes) noexcept;
};


AFIO_V2_NAMESPACE_END

#if AFIO_HEADERS_ONLY == 1 && !defined(DOXYGEN_SHOULD_SKIP_THIS)
#define AFIO_INCLUDED_BY_HEADER 1
#ifdef _WIN32
#include "detail/impl/windows/handle.ipp"
#else
#include "detail/impl/posix/handle.ipp"
#endif
#undef AFIO_INCLUDED_BY_HEADER
#endif

#ifdef _MSC_VER
#pragma warning(pop)
#endif

#endif