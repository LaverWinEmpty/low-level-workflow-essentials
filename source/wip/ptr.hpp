#ifndef LWE_MEM_PTR
#define LWE_MEM_PTR

#include "C:/Git/working/source/base/base.h"

LWE_BEGIN
namespace mem {

	//! @brief non-thread-safe unique_ptr with weak-copy semantics.
	//! @note  call clone() to make this instance independent.
	template<typename T> class Ptr {
		struct Handle {
			Handle* prev = nullptr;
			Handle* next = nullptr;
		};

		struct Handler {
			~Handler() { /* not work */ }
			union {
				T       data;
				uint8_t ptr[sizeof(T)] = { 0 }; // memset
			};
			Handle* head = nullptr;

		public:
			bool unique() {
				return head == nullptr || head->next == nullptr;
			}

		public:
			void link(Handle* in) {
				in->prev = nullptr; // [null]<--[ in ]-->[????]
				in->next = head;    // [null]<--[ in ]-->[head]
				if (head) head->prev = in;      // [null]<->[ in ]<->[head]
				head = in;      // [null]<->[head]<->[....]
			}

		public:
			void unlink(Handle* in) {
				if (in == head) {      // [head]<->[....]<->[....]
					head = head->next; // [ in ]<->[head]<->[....]
				}

				if (in->prev) in->prev->next = in->next; // [prev]<--[ in ]-->[next] 
				if (in->next) in->next->prev = in->prev; //    ^----------------^

				in->prev = nullptr; // [prev]   [ in ]-->[next]
				in->next = nullptr; // [prev]<---------->[next]
			}
		};

	public:
		template<typename... Args> Ptr(Args&&... in) {
			handle = static_cast<Handle*>(std::malloc(sizeof(Handle)));
			handler = static_cast<Handler*>(std::malloc(sizeof(Handler)));

			// check
			if (handle == nullptr || handler == nullptr) {
				if (handle) {
					std::free(handle);
					handle = nullptr;
				}
				if (handler) {
					std::free(handler);
					handler = nullptr;
				}
				throw std::bad_alloc(); // failed
			}

			// init
			else {
				new (handler) Handler();
				new (handle)  Handle();
			}

			handler->link(handle); // add handle
			new (handler->ptr) T(std::forward<Args>(in)...); // placement new
		}

	public:
		Ptr(const Ptr& in) {
			handle = static_cast<Handle*>(std::malloc(sizeof(Handle)));
			if (handle == nullptr) {
				throw std::bad_alloc(); // failed
			}
			else new (handle) Handle(); // init

			handler = in.handler;  // shared
			handler->link(handle); // add handle
		}

	public:
		Ptr(Ptr&& in) noexcept : handler(in.handler), handle(in.handle) {
			in.handler = nullptr;
			in.handle = nullptr;
		}

	public:
		~Ptr() {
			if (handler) {
				handler->unlink(handle); // pop

				// null == empty
				if (handler->head == nullptr) {
					handler->data.~T(); // call deconstructor
					free(handler);      // Handler destructor not required
				}
			}

			// check for safe
			if (handle) {
				free(handle); // Handle destructor not required
			}
		}

	public:
		void clone() {
			// unique
			if (handler->unique() == true) {
				return;
			}

			//  new
			Handler* newly = new Handler; // for deep copy
			new (newly->ptr) T(handler->data); // copy uses placement new

			// move
			handler->unlink(handle); // pop
			handler = newly;         // new
			handler->link(handle);   // set
		}

	public:
		T* operator->() {
			return &handler->data;
		}

		T& operator*() {
			return handler->data;
		}

		const T& operator*() const {
			return handler->data;
		}

	private:
		Handler* handler;
		Handle* handle;
	};

}
LWE_END
#endif