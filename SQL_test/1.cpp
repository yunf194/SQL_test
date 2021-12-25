#pragma comment(lib, "libmysql.lib")
#pragma warning(disable : 4996)
#include <stdio.h>
#include "mysql.h"
#include<iostream>
#include<iomanip>
#include<string>
#include<cstdlib>
using namespace std;
MYSQL m_sqlCon; //һ�����ݿ�ṹ��
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
	
	if (mysql_init(&m_sqlCon) == NULL) {//��ʼ�����ݿ�
		cout << "��ʼ�����ݿ�ʧ��" << endl;
		return false;
	}
	
	mysql_options(&m_sqlCon, MYSQL_SET_CHARSET_NAME, "gbk");//���ñ��뷽ʽ
	// localhost:������ rootΪ�˺����� testΪ���ݿ��� 3306Ϊ�˿�
	if (mysql_real_connect(&m_sqlCon, "localhost", "fengyun", "942365", "csedb_u201916464", 3306, NULL, 0) == NULL) {
		cout << (mysql_error(&m_sqlCon));//�ж��������ʧ�ܾͰ�����ʧ�ܵ���Ϣ��ʾ���������Ǻý��ж�Ӧ�޸ġ�
		return false;
	}
	cout << "���ݿ����ӳɹ�" << endl;
	return true;
}
void selectQuery(string s) {
	if (mysql_query(&m_sqlCon, s.c_str())) {
		cout << "��ѯʧ��" << endl;
		return;
	}
	cout << "��ѯ�ɹ�" << endl;
	MYSQL_RES* result; //һ��������ṹ��
	result = mysql_store_result(&m_sqlCon);
	if (result) {
		int row_num, col_num;
		row_num = mysql_num_rows(result);
		col_num = mysql_num_fields(result);
		cout << "����" << row_num << "�����ݣ�����Ϊ����ϸ���ݣ�" << endl;
		MYSQL_FIELD* fd;
		while (fd = mysql_fetch_field(result)) {
			cout << left << setw(11) << fd->name;
		}
		cout << endl;
		MYSQL_ROW sql_row; //char** ��ά���飬���һ������¼
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
		string tmp = no;//���ΪSno��Cno
		string Sno = tmp.substr(0, 9);
		string Cno = tmp.substr(9);
		sprintf(select_user, "select * from %s where Sno='%s' and Cno='%s'",table, Sno.c_str(), Cno.c_str());
	}
	if (mysql_query(&m_sqlCon, select_user) || !(result = mysql_store_result(&m_sqlCon))) {
		cout << "��ѯʧ��" << endl;
		return 0;
	}
	if (mysql_num_rows(result)) {
		cout << "��ǰ��Ϣ�Ѿ�����...." ;
		mysql_free_result(result);
		return 1;
	}
	else {
		cout << "�õ�ǰ��Ϣ������....";
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
		cout << "��������ʧ��" << endl;
		return;
	}
	cout << "�������ݳɹ�,�����룺" << mysql_affected_rows(&m_sqlCon) << "��" << endl;
}
void stuUpdate(StuBody &body) {
	char query[255];
	if (checkExist("student", body.Sno) == -1)return;
	sprintf(query, "update student set Sname='%s',Ssex='%s',Sage=%d,Sdept='%s',Scholarship='%s' where Sno ='%s'",
		body.Sname, body.Ssex, body.Sage, body.Sdept, body.Scholarship, body.Sno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "�޸�ʧ��" << endl;
		return;
	}
	cout << "�޸ĳɹ������޸ģ�" << mysql_affected_rows(&m_sqlCon) << "��" << endl;
}
void stuDelete(char* sno) {
	char query[255];
	if (checkExist("student", sno) == -1)return;
	sprintf(query, "delete from student where sno='%s'", sno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "ɾ��ʧ��" << endl;
		return;
	}
	cout << "ɾ���ɹ�����ɾ����" << mysql_affected_rows(&m_sqlCon) << "��" << endl;
}
void courseInsert(Course &course) {
	char query[255];
	if (checkExist("course", course.Cno) == 1)return;
	sprintf(query, "insert into course(Cno,Cname,Cpno,Ccredit) values('%s','%s','%s',%d)",
		course.Cno, course.Cname, course.Cpno, course.Ccredit);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "��������ʧ��" << endl;
		return;
	}
	cout << "�������ݳɹ�,�����룺" << mysql_affected_rows(&m_sqlCon) << "��" << endl;
}
void courseUpdate(Course& course) {
	char query[255];
	if (checkExist("course", course.Cno) == -1)return;
	sprintf(query, "update course set Cname='%s',Cpno='%s',Ccredit='%d' where Cno ='%s'",
		course.Cname, course.Cpno, course.Ccredit, course.Cno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "�޸�ʧ��" << endl;
		return;
	}
	cout << "�޸ĳɹ������޸ģ�" << mysql_affected_rows(&m_sqlCon) << "��" << endl;
}
void courseDelete(char* cno) {
	char query[255];
	if (checkExist("course", cno) == -1)return;
	sprintf(query, "delete from course where Cno='%s'", cno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "ɾ��ʧ��" << endl;
		return;
	}
	cout << "ɾ���ɹ�����ɾ����" << mysql_affected_rows(&m_sqlCon) << "��" << endl;
}
void gradeInsert(const Sc& sc) {
	
	char query[255];
	//Sno��Cno���Ϊ����
	char scno[50];
	strcpy(scno, sc.Sno);
	strcat(scno, sc.Cno);
	if (checkExist("sc", scno) == 1)return;
	sprintf(query, "insert into sc(Sno,Cno,Grade) values('%s','%s',%d)",
		sc.Sno,sc.Cno, sc.Grade);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "��������ʧ��" << endl;
		return;
	}
	cout << "�������ݳɹ�,�����룺" << mysql_affected_rows(&m_sqlCon) << "��" << endl;
}
void gradeUpdate(Sc& sc) {
	char query[255];
	//Sno��Cno���Ϊ����
	char scno[50];
	strcpy(scno, sc.Sno);
	strcat(scno, sc.Cno);
	if (checkExist("sc", scno) == -1)return;
	sprintf(query, "update sc set Grade=%d where Sno ='%s' and Cno='%s'",
		sc.Grade,sc.Sno,sc.Cno);
	if (mysql_query(&m_sqlCon, query)) {
		cout << "�޸�ʧ��" << endl;
		return;
	}
	cout << "�޸ĳɹ������޸ģ�" << mysql_affected_rows(&m_sqlCon) << "��" << endl;
}
int main()
{
	if (initConnection() == 0)return 0;
	int op = 0;
	char sno[10], query[250];
	do{
		cout << "---------------csedb_u201916464���ݿ����ϵͳ------------" << endl;
		cout << "---------------1.ѧ����Ϣά��----------------------------" << endl;
		cout << "---------------2.�γ���Ϣά��----------------------------" << endl;
		cout << "---------------3.ѧ���ɼ�ά��----------------------------" << endl;
		cout << "---------------4.��ϵͳ��ѧ���ɼ��������ʲ�������----------" << endl;
		cout << "---------------5.��ϵ��ɼ���ʾѧ��������Ϣ---------------" << endl;
		cout << "---------------6.�鿴ѧ����Ϣ��ѡ����Ϣ--------------------" << endl;
		cout << "---------------7.�˳����ݿ����ϵͳ-----------------------" << endl;
		cin >> op;
		switch (op)
		{
		case 1:
			system("cls");
			cout << "0.�˳�ѧ����Ϣά��\n1.����ѧ����Ϣ\n2.�޸�ѧ����Ϣ\n3.ɾ��ѧ����Ϣ\n";
			while (cin >> op) {
				if (op == 0)break;
				else if (op == 1) {
					cout << "������ѧ����Ϣ:ѧ�ţ��������Ա����䣬ϵ����ѧ��" << endl;
					StuBody stu(cin);
					stuInsert(stu);
				}
				else if (op == 2) {
					cout << "������ѧ����Ϣ:ѧ�ţ��������Ա����䣬ϵ����ѧ��" << endl;
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
			cout << "0.�˳��γ���Ϣά��\n1.�����¿γ�\n2.�޸Ŀγ���Ϣ\n3.ɾ���γ���Ϣ\n";
			while (cin >> op) {
				if (op == 0)break;
				else if (op == 1) {
					cout << "������γ���Ϣ���γ̱�ţ��γ�����ǰ�ÿγ̱�ţ�ѧ��" << endl;
					Course course(cin);
					courseInsert(course);
				}
				else if (op == 2) {
					cout << "������γ���Ϣ:�γ̱�ţ��γ�����ǰ�ÿγ̱�ţ�ѧ��" << endl;
					Course course(cin);
					courseUpdate(course);
				}
				else if (op == 3) {
					cout << "������γ���Ϣ:�γ̱��" << endl;
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
			cout << "0.�˳�ѧ���ɼ�ά��\n1.¼���µĳɼ�\n2.�޸�ѧ���ɼ�\n3.ɾ��ѧ���ɼ�\n";
			while (cin >> op) {
				if (op == 0)break;
				else if (op == 1) {
					cout << "������ѧ���ɼ���Ϣ��ѧ��ѧ�ţ��γ̱�ţ��ɼ�" << endl;
					Sc sc(cin);
					gradeInsert(sc);
				}
				else if (op == 2) {
					cout << "������ѧ���ɼ���Ϣ��ѧ��ѧ�ţ��γ̱�ţ��ɼ�" << endl;
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
			selectQuery("SELECT Sdept,AVG(Grade) AS avg_grade,MAX(Grade) AS max_grade,MIN(Grade) AS min_grade,100*COUNT(IF(Grade >=90, 1, NULL))/COUNT(*) AS '������%',COUNT(IF(Grade<60,1,NULL)) AS '����������' FROM student,sc WHERE student.`Sno`=sc.`Sno` GROUP BY Sdept");
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
			cout << "����ѧ����ѧ�ţ�\n";
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
	//ͣ���ȴ�
	system("pause");
	return 0;
}