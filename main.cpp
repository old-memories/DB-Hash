#include <iostream>
#include <cstdlib>
#include<ctime>
#include<map>					 //测试时存储随机字符串
#include "Hash.h"
#define TEST_NUM 1000000
using namespace std;


char* randomString(int max);//make random string  length:(1~max)
void test_correct();
void test_performance();
void run();
int main() {
	cout << "————————————菜单————————————\n";
	cout << "输入1进行演示小测试，输入2进行性能测试，输入0进入交互界面，输入-1退出程序\n";
	int num;
	cin >> num;
	while (num != -1) {
		if (num == 1)
			test_correct();
		if(num==2)
			test_performance();
		if (num == 0)
			run();
		cout << "————————————菜单————————————\n";
		cout << "输入1进行演示小测试，输入2进行性能测试，输入0进入交互界面，输入-1退出程序\n";
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
	cout << "插入测试\n";
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
	cout << "查询测试\n";
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
	cout << "修改测试\n";
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
	cout << "删除测试\n";
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
	cout << "插入three-3,two-2,four-4三对键值对到数据库\n";
	db.insert("three", "3");
	db.insert("two", "2");
	db.insert("four", "4");
	cout << "打印所有键值对\n";
	db.traversal();
	cout << "查找two和three对应的value\n";
	if (char* value = db.find("two"))
		cout << "value is: " << value << endl;
	if (char* value = db.find("three"))
		cout << "value is: " << value << endl;
	cout << "替换two对应的value为er\n";
	if (db.replace("two", "er")) {
		if (char* value = db.find("two"))
			cout << "value is: " << value << endl;
	}
	cout << "删除four键值对\n";
	if (db.del("four")) {
		if (!db.find("four"))
			cout << "four已被删除\n";
	}
	cout << "打印所有键值对\n";
	db.traversal();
	cout << "插入four-4键值对\n";
	db.insert("four", "4");
	cout << "查找four对应的value\n";
	if (char* value = db.find("four"))
		cout << "value is: " << value << endl;
	cout << "打印所有键值对\n";
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
	cout << "输入0进行随机字符串初始化操作，输入1进行插入操作，输入2进行查找操作，输入3进行替换操作，输入4进行删除操作，输入5打印整个数据库，输入-1返回上级菜单\n";
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
			cout << "插入操作\n";
			cout << "输入key：\n";
			cin >> key;
			cout << "输入value：\n";
			cin >> value;
			if (db.insert(key, value))
				cout << "插入操作完成\n";
			else
				cout << "无法插入\n";
		}
		if (n == 2) {
			char key[KEYSIZE_MAX];
			cout << "查找操作\n";
			cout << "输入key：\n";
			cin >> key;
			if (char*value = db.find(key)) {
				cout << "value is: " << value << endl;
				cout << "查找操作完成\n";
			}
			else
				cout<<"找不到对应value\n";
			
		}
		if (n == 3) {
			char key[KEYSIZE_MAX];
			char value[VALUESIZE_MAX];
			cout << "替换操作\n";
			cout << "输入key：\n";
			cin >> key;
			cout << "输入value：\n";
			cin >> value;
			if(db.replace(key, value))
				cout << "替换操作完成\n";
			else
				cout << "无法替换\n";
		}
		if (n == 4) {
			char key[KEYSIZE_MAX];
			cout << "删除操作\n";
			cout << "输入key：\n";
			cin >> key;
			if ( db.del(key)) {
				cout << "删除操作完成\n";
			}
			else
				cout << "找不到该key\n";
		}
		if (n == 5) {
			cout << "打印操作\n";
			db.traversal();
			cout << "打印操作完成\n";
		}
		cout << "输入1进行插入操作，输入2进行查找操作，输入3进行替换操作，输入4进行删除操作，输入5打印整个数据库，输入-1返回上级菜单\n";
		cin >> n;
		
	}
	db.close();
}