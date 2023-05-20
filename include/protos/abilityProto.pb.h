// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: abilityProto.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_abilityProto_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_abilityProto_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021009 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_abilityProto_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_abilityProto_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_abilityProto_2eproto;
namespace abilityUnit {
class ConnectInfo;
struct ConnectInfoDefaultTypeInternal;
extern ConnectInfoDefaultTypeInternal _ConnectInfo_default_instance_;
class DisconnectInfo;
struct DisconnectInfoDefaultTypeInternal;
extern DisconnectInfoDefaultTypeInternal _DisconnectInfo_default_instance_;
class Response;
struct ResponseDefaultTypeInternal;
extern ResponseDefaultTypeInternal _Response_default_instance_;
class StartInfo;
struct StartInfoDefaultTypeInternal;
extern StartInfoDefaultTypeInternal _StartInfo_default_instance_;
class TerminateInfo;
struct TerminateInfoDefaultTypeInternal;
extern TerminateInfoDefaultTypeInternal _TerminateInfo_default_instance_;
}  // namespace abilityUnit
PROTOBUF_NAMESPACE_OPEN
template<> ::abilityUnit::ConnectInfo* Arena::CreateMaybeMessage<::abilityUnit::ConnectInfo>(Arena*);
template<> ::abilityUnit::DisconnectInfo* Arena::CreateMaybeMessage<::abilityUnit::DisconnectInfo>(Arena*);
template<> ::abilityUnit::Response* Arena::CreateMaybeMessage<::abilityUnit::Response>(Arena*);
template<> ::abilityUnit::StartInfo* Arena::CreateMaybeMessage<::abilityUnit::StartInfo>(Arena*);
template<> ::abilityUnit::TerminateInfo* Arena::CreateMaybeMessage<::abilityUnit::TerminateInfo>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace abilityUnit {

// ===================================================================

class Response final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:abilityUnit.Response) */ {
 public:
  inline Response() : Response(nullptr) {}
  ~Response() override;
  explicit PROTOBUF_CONSTEXPR Response(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Response(const Response& from);
  Response(Response&& from) noexcept
    : Response() {
    *this = ::std::move(from);
  }

  inline Response& operator=(const Response& from) {
    CopyFrom(from);
    return *this;
  }
  inline Response& operator=(Response&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Response& default_instance() {
    return *internal_default_instance();
  }
  static inline const Response* internal_default_instance() {
    return reinterpret_cast<const Response*>(
               &_Response_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Response& a, Response& b) {
    a.Swap(&b);
  }
  inline void Swap(Response* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Response* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Response* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Response>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Response& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const Response& from) {
    Response::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Response* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "abilityUnit.Response";
  }
  protected:
  explicit Response(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kMsgFieldNumber = 2,
    kCodeFieldNumber = 1,
  };
  // string msg = 2;
  void clear_msg();
  const std::string& msg() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_msg(ArgT0&& arg0, ArgT... args);
  std::string* mutable_msg();
  PROTOBUF_NODISCARD std::string* release_msg();
  void set_allocated_msg(std::string* msg);
  private:
  const std::string& _internal_msg() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_msg(const std::string& value);
  std::string* _internal_mutable_msg();
  public:

  // int32 code = 1;
  void clear_code();
  int32_t code() const;
  void set_code(int32_t value);
  private:
  int32_t _internal_code() const;
  void _internal_set_code(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:abilityUnit.Response)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr msg_;
    int32_t code_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_abilityProto_2eproto;
};
// -------------------------------------------------------------------

class StartInfo final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:abilityUnit.StartInfo) */ {
 public:
  inline StartInfo() : StartInfo(nullptr) {}
  ~StartInfo() override;
  explicit PROTOBUF_CONSTEXPR StartInfo(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  StartInfo(const StartInfo& from);
  StartInfo(StartInfo&& from) noexcept
    : StartInfo() {
    *this = ::std::move(from);
  }

  inline StartInfo& operator=(const StartInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline StartInfo& operator=(StartInfo&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const StartInfo& default_instance() {
    return *internal_default_instance();
  }
  static inline const StartInfo* internal_default_instance() {
    return reinterpret_cast<const StartInfo*>(
               &_StartInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  friend void swap(StartInfo& a, StartInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(StartInfo* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(StartInfo* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  StartInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<StartInfo>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const StartInfo& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const StartInfo& from) {
    StartInfo::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(StartInfo* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "abilityUnit.StartInfo";
  }
  protected:
  explicit StartInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTimestampFieldNumber = 1,
  };
  // int32 timestamp = 1;
  void clear_timestamp();
  int32_t timestamp() const;
  void set_timestamp(int32_t value);
  private:
  int32_t _internal_timestamp() const;
  void _internal_set_timestamp(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:abilityUnit.StartInfo)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    int32_t timestamp_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_abilityProto_2eproto;
};
// -------------------------------------------------------------------

class ConnectInfo final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:abilityUnit.ConnectInfo) */ {
 public:
  inline ConnectInfo() : ConnectInfo(nullptr) {}
  ~ConnectInfo() override;
  explicit PROTOBUF_CONSTEXPR ConnectInfo(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  ConnectInfo(const ConnectInfo& from);
  ConnectInfo(ConnectInfo&& from) noexcept
    : ConnectInfo() {
    *this = ::std::move(from);
  }

  inline ConnectInfo& operator=(const ConnectInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline ConnectInfo& operator=(ConnectInfo&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const ConnectInfo& default_instance() {
    return *internal_default_instance();
  }
  static inline const ConnectInfo* internal_default_instance() {
    return reinterpret_cast<const ConnectInfo*>(
               &_ConnectInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  friend void swap(ConnectInfo& a, ConnectInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(ConnectInfo* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(ConnectInfo* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  ConnectInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<ConnectInfo>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const ConnectInfo& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const ConnectInfo& from) {
    ConnectInfo::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ConnectInfo* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "abilityUnit.ConnectInfo";
  }
  protected:
  explicit ConnectInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kIpFieldNumber = 1,
    kPortFieldNumber = 2,
    kTimestampFieldNumber = 3,
  };
  // string ip = 1;
  void clear_ip();
  const std::string& ip() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_ip(ArgT0&& arg0, ArgT... args);
  std::string* mutable_ip();
  PROTOBUF_NODISCARD std::string* release_ip();
  void set_allocated_ip(std::string* ip);
  private:
  const std::string& _internal_ip() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_ip(const std::string& value);
  std::string* _internal_mutable_ip();
  public:

  // int32 port = 2;
  void clear_port();
  int32_t port() const;
  void set_port(int32_t value);
  private:
  int32_t _internal_port() const;
  void _internal_set_port(int32_t value);
  public:

  // int32 timestamp = 3;
  void clear_timestamp();
  int32_t timestamp() const;
  void set_timestamp(int32_t value);
  private:
  int32_t _internal_timestamp() const;
  void _internal_set_timestamp(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:abilityUnit.ConnectInfo)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr ip_;
    int32_t port_;
    int32_t timestamp_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_abilityProto_2eproto;
};
// -------------------------------------------------------------------

class DisconnectInfo final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:abilityUnit.DisconnectInfo) */ {
 public:
  inline DisconnectInfo() : DisconnectInfo(nullptr) {}
  ~DisconnectInfo() override;
  explicit PROTOBUF_CONSTEXPR DisconnectInfo(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  DisconnectInfo(const DisconnectInfo& from);
  DisconnectInfo(DisconnectInfo&& from) noexcept
    : DisconnectInfo() {
    *this = ::std::move(from);
  }

  inline DisconnectInfo& operator=(const DisconnectInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline DisconnectInfo& operator=(DisconnectInfo&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const DisconnectInfo& default_instance() {
    return *internal_default_instance();
  }
  static inline const DisconnectInfo* internal_default_instance() {
    return reinterpret_cast<const DisconnectInfo*>(
               &_DisconnectInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    3;

  friend void swap(DisconnectInfo& a, DisconnectInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(DisconnectInfo* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(DisconnectInfo* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  DisconnectInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<DisconnectInfo>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const DisconnectInfo& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const DisconnectInfo& from) {
    DisconnectInfo::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(DisconnectInfo* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "abilityUnit.DisconnectInfo";
  }
  protected:
  explicit DisconnectInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTimestampFieldNumber = 1,
  };
  // int32 timestamp = 1;
  void clear_timestamp();
  int32_t timestamp() const;
  void set_timestamp(int32_t value);
  private:
  int32_t _internal_timestamp() const;
  void _internal_set_timestamp(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:abilityUnit.DisconnectInfo)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    int32_t timestamp_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_abilityProto_2eproto;
};
// -------------------------------------------------------------------

class TerminateInfo final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:abilityUnit.TerminateInfo) */ {
 public:
  inline TerminateInfo() : TerminateInfo(nullptr) {}
  ~TerminateInfo() override;
  explicit PROTOBUF_CONSTEXPR TerminateInfo(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  TerminateInfo(const TerminateInfo& from);
  TerminateInfo(TerminateInfo&& from) noexcept
    : TerminateInfo() {
    *this = ::std::move(from);
  }

  inline TerminateInfo& operator=(const TerminateInfo& from) {
    CopyFrom(from);
    return *this;
  }
  inline TerminateInfo& operator=(TerminateInfo&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const TerminateInfo& default_instance() {
    return *internal_default_instance();
  }
  static inline const TerminateInfo* internal_default_instance() {
    return reinterpret_cast<const TerminateInfo*>(
               &_TerminateInfo_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    4;

  friend void swap(TerminateInfo& a, TerminateInfo& b) {
    a.Swap(&b);
  }
  inline void Swap(TerminateInfo* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(TerminateInfo* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  TerminateInfo* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<TerminateInfo>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const TerminateInfo& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const TerminateInfo& from) {
    TerminateInfo::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(TerminateInfo* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "abilityUnit.TerminateInfo";
  }
  protected:
  explicit TerminateInfo(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kTimestampFieldNumber = 1,
  };
  // int32 timestamp = 1;
  void clear_timestamp();
  int32_t timestamp() const;
  void set_timestamp(int32_t value);
  private:
  int32_t _internal_timestamp() const;
  void _internal_set_timestamp(int32_t value);
  public:

  // @@protoc_insertion_point(class_scope:abilityUnit.TerminateInfo)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    int32_t timestamp_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_abilityProto_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Response

// int32 code = 1;
inline void Response::clear_code() {
  _impl_.code_ = 0;
}
inline int32_t Response::_internal_code() const {
  return _impl_.code_;
}
inline int32_t Response::code() const {
  // @@protoc_insertion_point(field_get:abilityUnit.Response.code)
  return _internal_code();
}
inline void Response::_internal_set_code(int32_t value) {
  
  _impl_.code_ = value;
}
inline void Response::set_code(int32_t value) {
  _internal_set_code(value);
  // @@protoc_insertion_point(field_set:abilityUnit.Response.code)
}

// string msg = 2;
inline void Response::clear_msg() {
  _impl_.msg_.ClearToEmpty();
}
inline const std::string& Response::msg() const {
  // @@protoc_insertion_point(field_get:abilityUnit.Response.msg)
  return _internal_msg();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Response::set_msg(ArgT0&& arg0, ArgT... args) {
 
 _impl_.msg_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:abilityUnit.Response.msg)
}
inline std::string* Response::mutable_msg() {
  std::string* _s = _internal_mutable_msg();
  // @@protoc_insertion_point(field_mutable:abilityUnit.Response.msg)
  return _s;
}
inline const std::string& Response::_internal_msg() const {
  return _impl_.msg_.Get();
}
inline void Response::_internal_set_msg(const std::string& value) {
  
  _impl_.msg_.Set(value, GetArenaForAllocation());
}
inline std::string* Response::_internal_mutable_msg() {
  
  return _impl_.msg_.Mutable(GetArenaForAllocation());
}
inline std::string* Response::release_msg() {
  // @@protoc_insertion_point(field_release:abilityUnit.Response.msg)
  return _impl_.msg_.Release();
}
inline void Response::set_allocated_msg(std::string* msg) {
  if (msg != nullptr) {
    
  } else {
    
  }
  _impl_.msg_.SetAllocated(msg, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.msg_.IsDefault()) {
    _impl_.msg_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:abilityUnit.Response.msg)
}

// -------------------------------------------------------------------

// StartInfo

// int32 timestamp = 1;
inline void StartInfo::clear_timestamp() {
  _impl_.timestamp_ = 0;
}
inline int32_t StartInfo::_internal_timestamp() const {
  return _impl_.timestamp_;
}
inline int32_t StartInfo::timestamp() const {
  // @@protoc_insertion_point(field_get:abilityUnit.StartInfo.timestamp)
  return _internal_timestamp();
}
inline void StartInfo::_internal_set_timestamp(int32_t value) {
  
  _impl_.timestamp_ = value;
}
inline void StartInfo::set_timestamp(int32_t value) {
  _internal_set_timestamp(value);
  // @@protoc_insertion_point(field_set:abilityUnit.StartInfo.timestamp)
}

// -------------------------------------------------------------------

// ConnectInfo

// string ip = 1;
inline void ConnectInfo::clear_ip() {
  _impl_.ip_.ClearToEmpty();
}
inline const std::string& ConnectInfo::ip() const {
  // @@protoc_insertion_point(field_get:abilityUnit.ConnectInfo.ip)
  return _internal_ip();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void ConnectInfo::set_ip(ArgT0&& arg0, ArgT... args) {
 
 _impl_.ip_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:abilityUnit.ConnectInfo.ip)
}
inline std::string* ConnectInfo::mutable_ip() {
  std::string* _s = _internal_mutable_ip();
  // @@protoc_insertion_point(field_mutable:abilityUnit.ConnectInfo.ip)
  return _s;
}
inline const std::string& ConnectInfo::_internal_ip() const {
  return _impl_.ip_.Get();
}
inline void ConnectInfo::_internal_set_ip(const std::string& value) {
  
  _impl_.ip_.Set(value, GetArenaForAllocation());
}
inline std::string* ConnectInfo::_internal_mutable_ip() {
  
  return _impl_.ip_.Mutable(GetArenaForAllocation());
}
inline std::string* ConnectInfo::release_ip() {
  // @@protoc_insertion_point(field_release:abilityUnit.ConnectInfo.ip)
  return _impl_.ip_.Release();
}
inline void ConnectInfo::set_allocated_ip(std::string* ip) {
  if (ip != nullptr) {
    
  } else {
    
  }
  _impl_.ip_.SetAllocated(ip, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.ip_.IsDefault()) {
    _impl_.ip_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:abilityUnit.ConnectInfo.ip)
}

// int32 port = 2;
inline void ConnectInfo::clear_port() {
  _impl_.port_ = 0;
}
inline int32_t ConnectInfo::_internal_port() const {
  return _impl_.port_;
}
inline int32_t ConnectInfo::port() const {
  // @@protoc_insertion_point(field_get:abilityUnit.ConnectInfo.port)
  return _internal_port();
}
inline void ConnectInfo::_internal_set_port(int32_t value) {
  
  _impl_.port_ = value;
}
inline void ConnectInfo::set_port(int32_t value) {
  _internal_set_port(value);
  // @@protoc_insertion_point(field_set:abilityUnit.ConnectInfo.port)
}

// int32 timestamp = 3;
inline void ConnectInfo::clear_timestamp() {
  _impl_.timestamp_ = 0;
}
inline int32_t ConnectInfo::_internal_timestamp() const {
  return _impl_.timestamp_;
}
inline int32_t ConnectInfo::timestamp() const {
  // @@protoc_insertion_point(field_get:abilityUnit.ConnectInfo.timestamp)
  return _internal_timestamp();
}
inline void ConnectInfo::_internal_set_timestamp(int32_t value) {
  
  _impl_.timestamp_ = value;
}
inline void ConnectInfo::set_timestamp(int32_t value) {
  _internal_set_timestamp(value);
  // @@protoc_insertion_point(field_set:abilityUnit.ConnectInfo.timestamp)
}

// -------------------------------------------------------------------

// DisconnectInfo

// int32 timestamp = 1;
inline void DisconnectInfo::clear_timestamp() {
  _impl_.timestamp_ = 0;
}
inline int32_t DisconnectInfo::_internal_timestamp() const {
  return _impl_.timestamp_;
}
inline int32_t DisconnectInfo::timestamp() const {
  // @@protoc_insertion_point(field_get:abilityUnit.DisconnectInfo.timestamp)
  return _internal_timestamp();
}
inline void DisconnectInfo::_internal_set_timestamp(int32_t value) {
  
  _impl_.timestamp_ = value;
}
inline void DisconnectInfo::set_timestamp(int32_t value) {
  _internal_set_timestamp(value);
  // @@protoc_insertion_point(field_set:abilityUnit.DisconnectInfo.timestamp)
}

// -------------------------------------------------------------------

// TerminateInfo

// int32 timestamp = 1;
inline void TerminateInfo::clear_timestamp() {
  _impl_.timestamp_ = 0;
}
inline int32_t TerminateInfo::_internal_timestamp() const {
  return _impl_.timestamp_;
}
inline int32_t TerminateInfo::timestamp() const {
  // @@protoc_insertion_point(field_get:abilityUnit.TerminateInfo.timestamp)
  return _internal_timestamp();
}
inline void TerminateInfo::_internal_set_timestamp(int32_t value) {
  
  _impl_.timestamp_ = value;
}
inline void TerminateInfo::set_timestamp(int32_t value) {
  _internal_set_timestamp(value);
  // @@protoc_insertion_point(field_set:abilityUnit.TerminateInfo.timestamp)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace abilityUnit

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_abilityProto_2eproto