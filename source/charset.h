#ifndef JY_CHARSET_H
#define JY_CHARSET_H


// �����ַ���ת��
// flag = 0   Big5 --> GBK     
//      = 1   GBK  --> Big5    
//      = 2   Big5 --> Unicode
//      = 3   GBK  --> Unicode
// ע��Ҫ��֤dest���㹻�Ŀռ䣬һ�㽨��ȡsrc���ȵ�����+1����֤ȫӢ���ַ�Ҳ��ת��Ϊunicode
int  JY_CharSet(const char *src, char *dst, int flag);

#endif // JY_CHARSET_H

