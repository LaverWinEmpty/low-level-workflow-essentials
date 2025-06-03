#ifdef LWE_META_REGISTRY

LWE_BEGIN
namespace meta {

template<typename T> T* Registry<T>::find(const char* in) {
   return find(string{ in });
}

template<typename T> T* Registry<T>::find(const string& in) {
   auto result = instance().find(in);
   if(result != instance().end()) {
       return result->second;
   }
   return nullptr;
}

template<typename T> template<typename U> void Registry<T>::add(const char* in) {
   add<U>(string{ in });
}

template<typename T> template<typename U> void Registry<T>::add(const string& in) {
   Table& table = instance();
   if (table.find(in) == table.end()) {
       table.insert({ in, static_cast<T*>(new U()) });
   }
}

template<typename T> Registry<T>::~Registry() {
   for(auto& it : table) {
       delete it.second;
   }
}

template<typename T> auto Registry<T>::instance()->Table& {
   static Registry<T> statics;
   return statics.table;
}

}
LWE_END
#endif