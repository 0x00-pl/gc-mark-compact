编码注意
===
1. 被gc的object应该由gc_manager_object_array_alloc()一系列函数申请
2. 被申请的对象是没有被初始化的, 需要object_xxx_init()来初始化, 对于数组,使用object_xxx_init_nth() .
3. 错误处理, 使用err_t **err 来配合宏使用. 不要在中途使用return在破坏结构, 改为goto fin .
4. 每个object_t*指针在触发gc的时候都是需要被重定向的. 使用gc_manager_stack_object_get_depth(), gc_manager_stack_object_push(), gc_manager_stack_object_balance() 这三个函数配合.
4.1. 已知bug: gc_manager_stack_object_push() 对某个指针使用两次的话, 这个指针会被重定向两次到一个错误的地址.
5. 当a是object_t*时, 不要写a.b= f(c)结构的代码. 如果f触发了gc,则a会被重定向, 导致未定义的行为.
