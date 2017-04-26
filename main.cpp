#include <iostream>
#include <cstdlib>
#include<ctime>
#include<map>					 //����ʱ�洢����ַ���
#include "Hash.h"
#define TEST_NUM 1000000
using namespace std;


char* randomString(int max);//make random string  length:(1~max)
void test_correct();
void test_performance();
void run();
int main() {
	cout << "�������������������������˵�������������������������\n";
	cout << "����1������ʾС���ԣ�����2�������ܲ��ԣ�����0���뽻�����棬����-1�˳�����\n";
	int num;
	cin >> num;
	while (num != -1) {
		if (num == 1)
			test_correct();
		if(num==2)
			test_performance();
		if (num == 0)
			run();
		cout << "�������������������������˵�������������������������\n";
		cout << "����1������ʾС���ԣ�����2�������ܲ��ԣ�����0���뽻�����棬����-1�˳�����\n";
		cin >> num;
	}
	return 0;
}

char* randomString(int max) {
	int len = rand() % max;
	len = ((len == 0) ? max : len);
	char* s = new char[max + 1];
	for (int j = 0; j < len; j++) {
		s[j] = 'a' + (rand() % 26);
	}
	s[len] = '\0';
	return s;
}

void test_performance() {
	string name = "test_performance";
	DB db(name);
	srand(time(NULL));
	map<char*, char*> m;
	map<char*, char*>::iterator it;
	for (int i = 0; i < TEST_NUM; i++) {
		char*key = randomString(KEYSIZE_MAX - 1);
		char*value = randomString(VALUESIZE_MAX - 1);
		m.insert(map<char*, char*>::value_type(key, value));
	}
	clock_t start;
	clock_t finish;
	cout << "�������\n";
	cout << "Test number: " << TEST_NUM << endl;
	start = clock();
	// insert test start 
	db.open();
	for (it = m.begin(); it != m.end(); it++) {
		db.insert(it->first, it->second);
	}
	db.close();
	finish = clock();
	
	// insert test end
	cout << "time for " << TEST_NUM << " insertion is: " << (double)(finish - start) / CLOCKS_PER_SEC << " seconds" << endl;
	cout << "��ѯ����\n";
	cout << "Test number: " << TEST_NUM << endl;
	start = clock();
	// find test start
	db.open();
	for (it = m.begin(); it != m.end(); it++) {
		db.find(it->first);
	}
	db.close();
	finish = clock();
	// find test end
	cout << "time for " << TEST_NUM << " finding is: " << (double)(finish - start) / CLOCKS_PER_SEC << " seconds" << endl;
	cout << "�޸Ĳ���\n";
	cout << "Test number: " << TEST_NUM << endl;
	start = clock();
	//replace test
	db.open();
	for (it = m.begin(); it != m.end(); it++) {
		char*value = randomString(VALUESIZE_MAX - 1);
		db.replace(it->first, value);
	}
	db.close();
	finish = clock();
	// replace test end
	cout << "time for " << TEST_NUM << " replacement is: " << (double)(finish - start) / CLOCKS_PER_SEC << " seconds" << endl;
	if (HASH_SIZE / TEST_NUM <= 100) {
		db.clear();
		return;
	}
	cout << "ɾ������\n";
	cout << "Test number: " << TEST_NUM << endl;
	start = clock();
	//delete test
	db.open();
	for (it = m.begin(); it != m.end(); it++) {
		db.del(it->first);
	}
	db.traversal();
	db.close();
	finish = clock();
	// delete test end
	cout << "time for " << TEST_NUM << " deletion is: " << (double)(finish - start) / CLOCKS_PER_SEC << " seconds" << endl;
	db.clear();
}



void test_correct() {
	string name = "test_correct";
	DB db(name);
	db.open();
	cout << "����three-3,two-2,four-4���Լ�ֵ�Ե����ݿ�\n";
	db.insert("three", "3");
	db.insert("two", "2");
	db.insert("four", "4");
	cout << "��ӡ���м�ֵ��\n";
	db.traversal();
	cout << "����two��three��Ӧ��value\n";
	if (char* value = db.find("two"))
		cout << "value is: " << value << endl;
	if (char* value = db.find("three"))
		cout << "value is: " << value << endl;
	cout << "�滻two��Ӧ��valueΪer\n";
	if (db.replace("two", "er")) {
		if (char* value = db.find("two"))
			cout << "value is: " << value << endl;
	}
	cout << "ɾ��four��ֵ��\n";
	if (db.del("four")) {
		if (!db.find("four"))
			cout << "four�ѱ�ɾ��\n";
	}
	cout << "��ӡ���м�ֵ��\n";
	db.traversal();
	cout << "����four-4��ֵ��\n";
	db.insert("four", "4");
	cout << "����four��Ӧ��value\n";
	if (char* value = db.find("four"))
		cout << "value is: " << value << endl;
	cout << "��ӡ���м�ֵ��\n";
	db.traversal();
	db.close();
	db.clear();
}


void run() {
	string name;
	cout << "Enter name for db:\n";
	cin >> name;
	DB db(name);
	db.open();
	cout << "����0��������ַ�����ʼ������������1���в������������2���в��Ҳ���������3�����滻����������4����ɾ������������5��ӡ�������ݿ⣬����-1�����ϼ��˵�\n";
	int n;
	cin >> n;
	while (n != -1) {
		if (n == 0) {
			srand(time(NULL));
			map<char*, char*> m;
			map<char*, char*>::iterator it;
			for (int i = 0; i < TEST_NUM; i++) {
				char*key = randomString(KEYSIZE_MAX - 1);
				char*value = randomString(VALUESIZE_MAX - 1);
				m.insert(map<char*, char*>::value_type(key, value));
			}
			for (it = m.begin(); it != m.end(); it++) {
				db.insert(it->first, it->second);
			}
		}
		if (n == 1) {
			char key[KEYSIZE_MAX];
			char value[VALUESIZE_MAX];
			cout << "�������\n";
			cout << "����key��\n";
			cin >> key;
			cout << "����value��\n";
			cin >> value;
			if (db.insert(key, value))
				cout << "����������\n";
			else
				cout << "�޷�����\n";
		}
		if (n == 2) {
			char key[KEYSIZE_MAX];
			cout << "���Ҳ���\n";
			cout << "����key��\n";
			cin >> key;
			if (char*value = db.find(key)) {
				cout << "value is: " << value << endl;
				cout << "���Ҳ������\n";
			}
			else
				cout<<"�Ҳ�����Ӧvalue\n";
			
		}
		if (n == 3) {
			char key[KEYSIZE_MAX];
			char value[VALUESIZE_MAX];
			cout << "�滻����\n";
			cout << "����key��\n";
			cin >> key;
			cout << "����value��\n";
			cin >> value;
			if(db.replace(key, value))
				cout << "�滻�������\n";
			else
				cout << "�޷��滻\n";
		}
		if (n == 4) {
			char key[KEYSIZE_MAX];
			cout << "ɾ������\n";
			cout << "����key��\n";
			cin >> key;
			if ( db.del(key)) {
				cout << "ɾ���������\n";
			}
			else
				cout << "�Ҳ�����key\n";
		}
		if (n == 5) {
			cout << "��ӡ����\n";
			db.traversal();
			cout << "��ӡ�������\n";
		}
		cout << "����1���в������������2���в��Ҳ���������3�����滻����������4����ɾ������������5��ӡ�������ݿ⣬����-1�����ϼ��˵�\n";
		cin >> n;
		
	}
	db.close();
}