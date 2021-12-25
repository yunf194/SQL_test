#pragma comment(lib, "libmysql.lib")
#pragma warning(disable : 4996)
#include <stdio.h>
#include "mysql.h"
#include<iostream>
#include<iomanip>
#include<string>
#include<cstdlib>
using namespace std;
MYSQL m_sqlCon; //一个数据库结构体
class StuBody{
public:
	char Sno[10];
	char Sname[9];
	char Ssex[3];
	int Sage;
	char Sdept[21];
	char Scholarship[3];
public:
	StuBody(char* sno,char*sname,char*ssex,int sage,char* sdept,char* scholarship) {
		strcpy(Sno, sno); strcpy(Sname, sname); strcpy(Ssex, ssex); 
		Sage = sage;
		strcpy(Sdept, sdept); strcpy(Scholarship, scholarship);
	}
	StuBody(const char* sno, const char* sname, const char* ssex, int sage, const char* sdept, const char* scholarship) {
		strcpy(Sno, sno); strcpy(Sname, sname); strcpy(Ssex, ssex);
		Sage = sage;
		strcpy(Sdept, sdept); strcpy(Scholarship, scholarship);
	}
	StuBody(istream& in) {
		in >> Sno >> Sname >> Ssex >> Sage >> Sdept >> Scholarship;
	}
};
class Course {
public:
	char Cno[5];
	char Cname[40];
	char Cpno[5];
	int Ccredit;
	Course(const char* cno,const char* cname,const char* cpno, int ccredit) {
		strcpy(Cno, cno); strcpy(Cname, cname); strcpy(Cpno, cpno);
		Ccredit = ccredit;
	}
	Course(istream& in) {
		in >> Cno >> Cname >> Cpno >> Ccredit;
	}
};
class Sc {
public:
	char Sno[10];
	char Cno[5];
	int Grade;
	Sc(const char* sno, const char* cno, int grade) {
		strcpy(Cno, cno); strcpy(Sno, sno);
		Grade = grade;
	}
	Sc(istream& in) {
		in >> Sno >> Cno >> Grade;
	}
};
bool initConnection() {
	
	if (mysql_init(&m_sqlCon) == NULL) {//初始化数据库
		cout << "初始化数据库失败" << endl;
		return false;
	}
	
	mysql_options(&m_sqlCon, MYSQL_SET_CHARSET_NAME, "gbk");//设置编码方式
	// localhost:服务器 root为账号密码 test为数据库名 3306为端口
	if (mysql_real_connect(&m_sqlCon, "localhost", "fengyun", "942365", "csedb_u201916464", 3306, NULL, 0) == NULL) {
		cout << (mysql_error(&m_sqlCon));//判断如果连接失败就把连接失败的信息显示出来，我们好进行对应修改。
		return false;
	}
	cout << "数据库连接成功" << endl;
	return true;
}
void selectQuery(string s) {
	if (mysql_query(&m_sqlCon, s.c_str())) {
		cout << "查询失败" << endl;
		return;
	}
	cout << "查询成功" << endl;
	MYSQL_RES* result; //一个结果集结构体
	result = mysql_store_result(&m_sqlCon);
	if (result) {
		int row_num, col_num;
		row_num = mysql_num_rows(result);
		col_num = mysql_num_fields(result);
		cout << "共有" << row_num << "条数据，以下为其详细内容：" << endl;
		MYSQL_FIELD* fd;
		while (fd = mysql_fetch_field(result)) {
			cout << left << setw(11) << fd->name;
		}
		cout << endl;
		MYSQL_ROW sql_row; //char** 二维数组，存放一条条记录
		while (sql_row = mysql_fetch_row(result)) {
			for (int i = 0; i < col_num; i++) {
				if (sql_row[i] == NULL) cout << setw(11) << "NULL";
				else cout << left << setw(11) << sql_row[i] ;
			}
			cout << endl;
		}
	}
	if (result != NULL)
		mysql_free_result(result);

}
int checkExist(const char* table,char* no) {
	MYSQL_RES* result;
	char select_user[200];
	if(table=="student")sprintf(select_user, "select * from %s where Sno='%s'", table, no);
	else if(table=="course")sprintf(select_user, "select * from %s where Cno='%s'", table, no);
	else if (table == "sc") {
		string tmp = no;//拆分为Sno与Cno
		string Sno = tmp.substr(0, 9);
		string Cno = tmp.substr(9);
		sprintf(select_user, "select * from %s where Sno='%s' and Cno='%s'",table, Sno.c_str(), Cno.c_str());
	}
	if (mysql_query(&m_sqlCon, select_user) || !(result = mysql_store_result(&m_sqlCon))) {
		cout << "查询失败" << endl;
		return 0;
	}
	if (mysql_num_rows(result)) {
		cout << "当前信息已经存在...." ;
		mysql_free_result(result);
		return 1;
	}
	else {
		cout << "该当前信息不存在....";
		mysql_free_result(result);
		return -1;
	}
	mysql_free_result(result);
}
void stuInsert(StuBody &body) {
	char query[255];
	if (checkExist("student", body.Sno) == 1)return;
	sprintf(query, "insert into student(Sno,Sname,Ssex,Sage,Sdept,Scholarship) values('%s','%s','%s',%d,'%s','%s')",
		body.Sno,body.Sname,body.Ssex,body.Sage,body.Sdept,body.Scholarship);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "插入数据失败" << endl;
		return;
	}
	cout << "插入数据成功,共插入：" << mysql_affected_rows(&m_sqlCon) << "行" << endl;
}
void stuUpdate(StuBody &body) {
	char query[255];
	if (checkExist("student", body.Sno) == -1)return;
	sprintf(query, "update student set Sname='%s',Ssex='%s',Sage=%d,Sdept='%s',Scholarship='%s' where Sno ='%s'",
		body.Sname, body.Ssex, body.Sage, body.Sdept, body.Scholarship, body.Sno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "修改失败" << endl;
		return;
	}
	cout << "修改成功，共修改：" << mysql_affected_rows(&m_sqlCon) << "行" << endl;
}
void stuDelete(char* sno) {
	char query[255];
	if (checkExist("student", sno) == -1)return;
	sprintf(query, "delete from student where sno='%s'", sno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "删除失败" << endl;
		return;
	}
	cout << "删除成功，共删除：" << mysql_affected_rows(&m_sqlCon) << "行" << endl;
}
void courseInsert(Course &course) {
	char query[255];
	if (checkExist("course", course.Cno) == 1)return;
	sprintf(query, "insert into course(Cno,Cname,Cpno,Ccredit) values('%s','%s','%s',%d)",
		course.Cno, course.Cname, course.Cpno, course.Ccredit);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "插入数据失败" << endl;
		return;
	}
	cout << "插入数据成功,共插入：" << mysql_affected_rows(&m_sqlCon) << "行" << endl;
}
void courseUpdate(Course& course) {
	char query[255];
	if (checkExist("course", course.Cno) == -1)return;
	sprintf(query, "update course set Cname='%s',Cpno='%s',Ccredit='%d' where Cno ='%s'",
		course.Cname, course.Cpno, course.Ccredit, course.Cno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "修改失败" << endl;
		return;
	}
	cout << "修改成功，共修改：" << mysql_affected_rows(&m_sqlCon) << "行" << endl;
}
void courseDelete(char* cno) {
	char query[255];
	if (checkExist("course", cno) == -1)return;
	sprintf(query, "delete from course where Cno='%s'", cno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "删除失败" << endl;
		return;
	}
	cout << "删除成功，共删除：" << mysql_affected_rows(&m_sqlCon) << "行" << endl;
}
void gradeInsert(const Sc& sc) {
	
	char query[255];
	//Sno与Cno组合为主键
	char scno[50];
	strcpy(scno, sc.Sno);
	strcat(scno, sc.Cno);
	if (checkExist("sc", scno) == 1)return;
	sprintf(query, "insert into sc(Sno,Cno,Grade) values('%s','%s',%d)",
		sc.Sno,sc.Cno, sc.Grade);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "插入数据失败" << endl;
		return;
	}
	cout << "插入数据成功,共插入：" << mysql_affected_rows(&m_sqlCon) << "行" << endl;
}
void gradeUpdate(Sc& sc) {
	char query[255];
	//Sno与Cno组合为主键
	char scno[50];
	strcpy(scno, sc.Sno);
	strcat(scno, sc.Cno);
	if (checkExist("sc", scno) == -1)return;
	sprintf(query, "update sc set Grade=%d where Sno ='%s' and Cno='%s'",
		sc.Grade,sc.Sno,sc.Cno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "修改失败" << endl;
		return;
	}
	cout << "修改成功，共修改：" << mysql_affected_rows(&m_sqlCon) << "行" << endl;
}
int main()
{
	if (initConnection() == 0)return 0;
	int op = 0;
	char sno[10], query[250];
	do{
		cout << "---------------csedb_u201916464数据库管理系统------------" << endl;
		cout << "---------------1.学生信息维护----------------------------" << endl;
		cout << "---------------2.课程信息维护----------------------------" << endl;
		cout << "---------------3.学生成绩维护----------------------------" << endl;
		cout << "---------------4.按系统计学生成绩及优秀率不及格率----------" << endl;
		cout << "---------------5.按系与成绩显示学生排名信息---------------" << endl;
		cout << "---------------6.查看学生信息与选课信息--------------------" << endl;
		cout << "---------------7.退出数据库管理系统-----------------------" << endl;
		cin >> op;
		switch (op)
		{
		case 1:
			system("cls");
			cout << "0.退出学生信息维护\n1.增添学生信息\n2.修改学生信息\n3.删除学生信息\n";
			while (cin >> op) {
				if (op == 0)break;
				else if (op == 1) {
					cout << "请输入学生信息:学号，姓名，性别，年龄，系，奖学金" << endl;
					StuBody stu(cin);
					stuInsert(stu);
				}
				else if (op == 2) {
					cout << "请输入学生信息:学号，姓名，性别，年龄，系，奖学金" << endl;
					StuBody stu(cin);
					stuUpdate(stu);
				}
				else if (op == 3) {
					break;
				}
			}
			system("pause");
			system("cls");
			break;
		case 2:
			system("cls");
			cout << "0.退出课程信息维护\n1.增添新课程\n2.修改课程信息\n3.删除课程信息\n";
			while (cin >> op) {
				if (op == 0)break;
				else if (op == 1) {
					cout << "请输入课程信息：课程编号，课程名，前置课程编号，学分" << endl;
					Course course(cin);
					courseInsert(course);
				}
				else if (op == 2) {
					cout << "请输入课程信息:课程编号，课程名，前置课程编号，学分" << endl;
					Course course(cin);
					courseUpdate(course);
				}
				else if (op == 3) {
					cout << "请输入课程信息:课程编号" << endl;
					char s[10];
					cin >> s;
					courseDelete(s);
				}
				else if (op == 4) {
					selectQuery("select * from course");
				}
			}
			system("pause");
			system("cls");
			break;
		case 3:
			system("cls");
			cout << "0.退出学生成绩维护\n1.录入新的成绩\n2.修改学生成绩\n3.删除学生成绩\n";
			while (cin >> op) {
				if (op == 0)break;
				else if (op == 1) {
					cout << "请输入学生成绩信息：学生学号，课程编号，成绩" << endl;
					Sc sc(cin);
					gradeInsert(sc);
				}
				else if (op == 2) {
					cout << "请输入学生成绩信息：学生学号，课程编号，成绩" << endl;
					Sc sc(cin);
					gradeUpdate(sc);
				}
				else if (op == 3) {
					selectQuery("select * from sc");
					break;
				}
			}
			system("pause");
			system("cls");
			break;
		case 4:
			selectQuery("SELECT Sdept,AVG(Grade) AS avg_grade,MAX(Grade) AS max_grade,MIN(Grade) AS min_grade,100*COUNT(IF(Grade >=90, 1, NULL))/COUNT(*) AS '优秀率%',COUNT(IF(Grade<60,1,NULL)) AS '不及格人数' FROM student,sc WHERE student.`Sno`=sc.`Sno` GROUP BY Sdept");
			system("pause");
			system("cls");
			break;
		case 5:
			selectQuery("SELECT student.*,course.*,sc.`Grade` FROM student LEFT JOIN sc ON student.`Sno`=sc.`Sno` LEFT JOIN course ON course.`Cno`=sc.`Cno` ORDER BY Sdept,Grade");
			system("pause");
			system("cls");
			break;
		case 6:
			system("cls");
			cout << "输入学生的学号：\n";
			cin >> sno;
			sprintf(query, "SELECT * FROM student, sc WHERE student.`Sno`=sc.`Sno` AND student.Sno = '%s'", sno);
			selectQuery(query);
			system("pause");
			system("cls");
			break;
		default:
			continue;
		}
	} while (op!=7);

	mysql_close(&m_sqlCon);
	//停留等待
	system("pause");
	return 0;
}