����ע��
===
1. ��gc��objectӦ����gc_manager_object_array_alloc()һϵ�к�������
2. ������Ķ�����û�б���ʼ����, ��Ҫobject_xxx_init()����ʼ��, ��������,ʹ��object_xxx_init_nth() .
3. ������, ʹ��err_t **err ����Ϻ�ʹ��. ��Ҫ����;ʹ��return���ƻ��ṹ, ��Ϊgoto fin .
4. ÿ��object_t*ָ���ڴ���gc��ʱ������Ҫ���ض����. ʹ��gc_manager_stack_object_get_depth(), gc_manager_stack_object_push(), gc_manager_stack_object_balance() �������������.
4.1. ��֪bug: gc_manager_stack_object_push() ��ĳ��ָ��ʹ�����εĻ�, ���ָ��ᱻ�ض������ε�һ������ĵ�ַ.
5. ��a��object_t*ʱ, ��Ҫдa.b= f(c)�ṹ�Ĵ���. ���f������gc,��a�ᱻ�ض���, ����δ�������Ϊ.
