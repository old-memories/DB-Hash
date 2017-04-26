#pragma once
#include <string>                  //���������ļ����������ļ�ʱʹ��
using namespace std;
#define KEYSIZE_MAX 8        //�����ַ�������󳤶�
#define VALUESIZE_MAX 8   //�����ַ�������󳤶�
#define HASH_SIZE  20000003 //ɢ�б�Ĵ�С
struct Idx {
	char key[KEYSIZE_MAX];     //�����ַ���
	unsigned int value_off;		  //�����������ļ���ƫ����
	unsigned int off;				  //�����������ļ���ƫ����
	unsigned int off_next;         //���������е�ǰ��������һ��������ƫ����
	int len_key;						  //�����ַ����ĳ���
	int len_value;                      //�����ַ����ĳ���
	bool isDelete;
};

class DB {
public:
	DB(string fileName);			                    //���������ļ��ҳ�ʼ�����ݿ⣬���ļ��Ѵ�����ʲô������
	unsigned int hash(const char* key);      //ɢ�к���
	int open();												//�����ݿ�������ļ���������last_idx_off��last_value_off����ֵ
	int close();                                             //����ر����ݿ�������ļ���������last_idx_off��last_value_off��ֵ���ļ�
	char* find(const char* key);                   //Ѱ��key��Ӧ��value������value�ַ���ָ��,���򷵻�NULL
	bool del(char* key);								//ɾ��key������true�����򷵻�false
	int insert(char* key, char* value);           //����key��value������1�����Ѵ��ڸ�key������0
	bool replace(char* key, char* value);      //�滻key��Ӧ��value���Ҳ���key����false���滻�ɹ�����true
	void traversal();                                      //��ӡ���ݿ�����key����Ӧvalue
	void clear();										 //ɾ�����ݿ�������ļ�
private:
	Idx* find_key(const char*key);                //Ѱ��key��Ӧ�����ṹ��,���ؽṹ���ָ�룬���򷵻�NULL
	unsigned int last_idx_off;                       //�����ļ���ƫ����
	unsigned int last_dat_off;                       //�����ļ���ƫ����
	string fileName;                                     //���ݿ���
	string idxName, datName;                      //�����ļ���
	FILE* fp1;                                                //�����ļ���
	FILE* fp2;                                               //�����ļ���
	errno_t err1, err2;                                    //fopen_s �ķ���ֵ�������ж����ļ��Ƿ����
};