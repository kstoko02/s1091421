#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <vector>
#include <string>
using namespace::std;

struct Date
{
	int year = 2000;
	int month = 0;
	int day = 0;
};

struct Reservation
{
	char phoneNumber[12] = ""; // phone number
	char name[8] = "";         // name
	Date date = Date();          // reservation date
	int time = 0;                // reservation time¡G1 for 11:30, 2 for 13:30,
								 //                   3 for 17:45, 4 for 19:45
	int partySize = 0;           // the number of customers for a reservation
	int menu = 0;                // 1 for NT$ 1080, 2 for NT$ 1680, 3 for NT$ 2280
};

struct AvailSeats
{
	Date date = Date(); // a date
	int numAvailSeats[5] = {};
}; // numAvailSeats[ 1 ]: the number of seats available for lunch #1 (11:30 ~ 13:30)
   // numAvailSeats[ 2 ]: the number of seats available for lunch #2 (13:30 ~ 15:30)
   // numAvailSeats[ 3 ]: the number of seats available for dinner #1 (17:45 ~ 19:45)
   // numAvailSeats[ 4 ]: the number of seats available for dinner #2 (19:45 ~ 21:45)

const int totalNumSeats = 15;

// array of days per month
int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// loads reservations from the file Reservations.dat
void loadReservations(vector< Reservation >& reservations);

// loads availSeats from the file AvailSeats.dat
void loadAvailSeats(vector< AvailSeats >& availSeats);

// initialize availSeats
void initializeAvailSeats(vector< AvailSeats >& availSeats);

// returns true if and only if date1 is earlier than date2
bool earlier(const Date& date1, const Date& date2);

// returns date1 - date2, i.e., the number of days between date1 and date2
int difference(const Date& date1, const Date& date2);

// input an integer in the range [ begin, end ]
int inputAnInteger(int begin, int end);

// make a reservation
void makeReservation(vector< Reservation >& reservations,
	vector< AvailSeats >& availSeats);

// compute the current date
void computeCurrentDate(Date& currentDate);

// returns true if and only if the specified year is a leap year
bool leapYear(int year);

// input reservation date
void inputDate(vector< AvailSeats >& availSeats, Date& date,
	const Date& currentDate, int partySize);

// returns currentDate + numDays
Date addition(const Date& currentDate, int numDays);

// returns true if and only if there are at least n seats available for date,
// where n = requiredSeats
bool hasAvailableSeats(const vector< AvailSeats >& availSeats, const Date date,
	int requiredSeats);

// returns true if availSeats[ i ].numAvailSeats[ timeCode ] >= requiredSeats,
// where availSeats[ i ].date == date
bool hasAvailableSeats(const vector< AvailSeats >& availSeats, const Date date,
	int timeCode, int requiredSeats);

// returns a nonnegative integer i if availSeats[ i ].date == date,
// returns -1 otherwise.
int findAvailSeat(const vector< AvailSeats >& availSeats, Date date);

// choose a reservation time
void inputTimeCode(const vector< AvailSeats >& availSeats, const Date date,
	int& timeCode, int partySize);

// displays partySize, date and time in reservation
void displayReservationInfo(const Reservation& reservation);

// availSeats[ i ].numAvailSeats[ timeCode ] -= requiredSeats,
// where availSeats[ i ].date == date
void decreaseAvailSeats(vector< AvailSeats >& availSeats, const Date date,
	int timeCode, int requiredSeats);

// input a phoneNumber,
// then displays partySize, date and time in all reservations for phoneNumber
void viewReservation(const vector< Reservation >& reservations);

// returns true if and only if there is an i such that
// reservations[ i ].phoneNumber == phoneNumber
bool exist(const vector< Reservation >& reservations, char phoneNumber[]);

// displays partySize, date and time in all reservations for phoneNumber
void displayReservationInfo(const vector< Reservation >& reservations,
	char phoneNumber[]);

// stores reservations into the file Reservations.dat
void storeReservations(vector< Reservation >& reservations);

// stores availSeats into the file AvailSeats.dat
void storeAvailSeats(vector< AvailSeats >& availSeats);

int main()
{
	cout << "Welcome to Zuo Zhe Zuo Sushi Wo Shou Si\n";

	vector< Reservation > reservations; // vector of all reservations
	vector< AvailSeats > availSeats;    // Vector of available seats

	loadReservations(reservations);
	initializeAvailSeats(availSeats);

	int choice;
	while (true)
	{
		cout << "\nEnter Your Choice\n"
			<< "1. Make Reservation\n"
			<< "2. Reservation Enquiry\n"
			<< "3. End Program\n";

		do cout << "? ";
		while ((choice = inputAnInteger(1, 3)) == -1);

		switch (choice)
		{
		case 1:
			makeReservation(reservations, availSeats);
			break;
		case 2:
			viewReservation(reservations);
			break;
		case 3:
			cout << "\nThank you. Goodbye.\n\n";
			storeReservations(reservations);
			storeAvailSeats(availSeats);
			system("pause");
			return 0;
		default:
			cerr << "Incorrect Choice!\n";
			break;
		}
	}

	system("pause");
}

void loadReservations(vector< Reservation >& reservations)
{
	ifstream infile;
	infile.open("Reservations.dat", ios::binary);
	if (!infile)
	{
		cout << "file could not be opened." << endl;
		exit(1);
		system("pause");
	}
	Reservation reservation;
	while (infile.read(reinterpret_cast<char*>(&reservation), sizeof(Reservation)))
	{
		reservations.push_back(reservation);
	}
	infile.close();
}

void loadAvailSeats(vector< AvailSeats >& availSeats)
{
	int num = 0;
	ifstream infile;
	infile.open("AvailSeats.dat", ios::binary);
	if (!infile)
	{
		cout << "file could not be opened." << endl;
		exit(1);
		system("pause");
	}
	AvailSeats availseat;
	while (!infile.eof())
	{
		infile.read(reinterpret_cast<char*>(&availseat), sizeof(AvailSeats));
		availSeats.push_back(availseat);
	}
	infile.close();
}

void initializeAvailSeats(vector< AvailSeats >& availSeats)
{
	Date currentDate;
	computeCurrentDate(currentDate);

	loadAvailSeats(availSeats);

	Date date;
	AvailSeats initialavailSeat;
	if (availSeats.size() == 0)
	{
		for (int i = 1; i <= 30; i++) 
		{
			initialavailSeat.date = addition(currentDate, i);

			for (int j = 1; j < 5; j++)
				initialavailSeat.numAvailSeats[j] = totalNumSeats;
			availSeats.push_back(initialavailSeat);
		}
	}
	else
	{
		int a = 0;
		if (earlier(availSeats[availSeats.size() - 1].date, currentDate))
		{
			for (size_t i = 0; i < availSeats.size(); i++)
			{
				initialavailSeat.date = addition(currentDate, i);

				for (int j = 1; j < 5; j++)
					initialavailSeat.numAvailSeats[j] = totalNumSeats;
				availSeats[i] = initialavailSeat;
			}
			for (int i = availSeats.size(); i <= 30; i++)
			{
				initialavailSeat.date = addition(currentDate, i);
				for (int j = 1; j < 5; j++)
					initialavailSeat.numAvailSeats[j] = totalNumSeats;
				availSeats.push_back(initialavailSeat);
			}
		}
		else if (earlier(availSeats[0].date, currentDate))
		{
		     a = difference(currentDate, availSeats[0].date);
			for (int c = 1; c <= a; c++)
			{
				for (int i = 0; i < availSeats.size() - 2; i++)
				{
					availSeats[i] = availSeats[i + 1];
				}
			}

			for (int i = 0; i < a; i++)
			{
				date = addition(date, 1);
				initialavailSeat.date = date;
				for (int i = 1; i <= 4; i++) initialavailSeat.numAvailSeats[i] = totalNumSeats;
				availSeats[availSeats.size() - a + i] = initialavailSeat;
			}
		}
	}
}

bool earlier(const Date& date1, const Date& date2)
{
	if (date1.year > date2.year)
	{
		return false;
	}
	else
	{
		if (date1.year < date2.year)
			return true;
		else
			if (date1.month > date2.month)
				return false;
			else if (date1.month < date2.month)
				return true;
			else
				if (date1.day < date2.day)
					return true;
				else
					return false;
	}
}

int difference(const Date& date1, const Date& date2)
{
	int a = 0;
	if (date1.year == date2.year)
	{
		if (date1.month != date2.month)
		{
			if (date1.month > date2.month)
			{
				a += date1.day - 1;
				a += days[date2.month] - date2.day + 1;
				return a;
			}
			if (date1.month < date2.month)
			{
				a += date2.day - 1;
				a += days[date1.month] - date1.day + 1;
				return a;
			}
		}
		else
		{
			if (date1.day > date2.day)
			{
				return date1.day - date2.day;
			}
			if (date1.day < date2.day)
			{
				return date2.day - date1.day;
			}
		}
	}
	else
	{
		a = date1.day + days[date2.month] - date2.day;
		if (a < 0)
			a = a * -1;
		return a;
	}
}

int inputAnInteger(int begin, int end)
{
	char string[80];
	cin >> string;
	if (strlen(string) == 0)
		exit(0);
	for (size_t i = 0; i < strlen(string); i++)
	{
		if (string[i] < '0' || string[i]>'9')
			return -1;
	}
	int choice = atoi(string);
	if (choice >= begin && choice <= end)
		return choice;
	else
		return -1;
}

void makeReservation(vector< Reservation >& reservations,
	vector< AvailSeats >& availSeats)
{
	Reservation newReservation;
	cout << endl;
	do cout << "Enter the party size (1~6): ";
	while ((newReservation.partySize = inputAnInteger(1, 6)) == -1);

	Date currentDate;
	computeCurrentDate(currentDate);

	inputDate(availSeats, newReservation.date, currentDate,
		newReservation.partySize);

	inputTimeCode(availSeats, newReservation.date, newReservation.time,
		newReservation.partySize);

	cout << "\nEnter name: ";
	cin >> newReservation.name;

	cout << "\nEnter phone Number: ";
	cin >> newReservation.phoneNumber;
	cin.ignore();

	cout << "\nChoose a menu:\n";
	cout << "1. NT$ 1080\n";
	cout << "2. NT$ 1680\n";
	cout << "3. NT$ 2280\n";

	do cout << "? ";
	while ((newReservation.menu = inputAnInteger(1, 3)) == -1);

	cout << endl;
	displayReservationInfo(newReservation);

	cout << "\nReservation Completed.\n";

	reservations.push_back(newReservation);

	decreaseAvailSeats(availSeats, newReservation.date, newReservation.time,
		newReservation.partySize);
}

void computeCurrentDate(Date& currentDate)
{
	int remainingSeconds = static_cast<int>(time(0)) + 8 * 60 * 60;

	currentDate.year = 1970;

	int secondsPerYear = 365 * 24 * 60 * 60;
	if (leapYear(currentDate.year))
		secondsPerYear += 24 * 60 * 60;

	while (remainingSeconds >= secondsPerYear)
	{
		remainingSeconds -= secondsPerYear;
		currentDate.year++;

		secondsPerYear = 365 * 24 * 60 * 60;
		if (leapYear(currentDate.year))
			secondsPerYear += 24 * 60 * 60;
	}

	int days[13] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
	if (leapYear(currentDate.year))
		days[2] = 29;

	currentDate.month = 1;
	while (remainingSeconds >= days[currentDate.month] * 24 * 60 * 60)
	{
		remainingSeconds -= days[currentDate.month] * 24 * 60 * 60;
		currentDate.month++;
	}

	currentDate.day = remainingSeconds / (24 * 60 * 60) + 1;
}

bool leapYear(int year)
{
	return (year % 400 == 0 || (year % 4 == 0 && year % 100 != 0));
}

void inputDate(vector< AvailSeats >& availSeats, Date& date,
	const Date& currentDate, int partySize)
{
	char string[30];
	int n = 0;
	int num = 0;
	int choice;
	cout << "Choose a date" << endl;
	for (size_t i = 1; i < 31; i++)
	{
		if (hasAvailableSeats(availSeats, availSeats[i].date, partySize) == true)
		{
			if (i < 10)
			cout << " ";
			cout << i << ". ";
			cout << availSeats[i].date.year << "/";
			if (availSeats[i].date.day > days[availSeats[i].date.month])
			{
				availSeats[i].date.month++;
				availSeats[i].date.day = 1;
			}
			if (availSeats[i].date.month < 10)
				cout << "0";
			cout << availSeats[i].date.month << "/";
			if (availSeats[i].date.day < 10)
				cout << "0";
			cout << availSeats[i].date.day << " ";
			num++;
			if (num % 4 == 0)
				cout << endl;
		}
		else
		{
			string[n] = i;
			n++;
		}
	}
	cout << endl;
	cout << "? ";
	while ((choice = inputAnInteger(1, 30)) == -1 || choice == string[0] || choice == string[1] || choice == string[2] || choice == string[3]
		|| choice == string[4] || choice == string[5] || choice == string[6] || choice == string[7] || choice == string[8] || choice == string[9]
		|| choice == string[10] || choice == string[11] || choice == string[12] || choice == string[13] || choice == string[14] || choice == string[15]
		|| choice == string[16] || choice == string[17] || choice == string[18] || choice == string[19] || choice == string[20] || choice == string[21]
		|| choice == string[22] || choice == string[23] || choice == string[24] || choice == string[25] || choice == string[26] || choice == string[27]
		|| choice == string[28] || choice == string[29]);

	date = availSeats[choice - 1].date;
}

Date addition(const Date& currentDate, int numDays)
{
	Date add;
	add.day = currentDate.day + numDays;
	add.month = currentDate.month;
	add.year = currentDate.year;
	if (add.day > days[currentDate.month])
	{
		add.month = currentDate.month + 1;
	    add.day = currentDate.day + numDays - days[currentDate.month];
	}
	if (add.month > 12)
	{
		add.year = currentDate.year + 1;
		add.month = currentDate.month - 12;
	}
	return add;
}

bool hasAvailableSeats(const vector< AvailSeats >& availSeats, const Date date,
	int requiredSeats)
{
	int pos = findAvailSeat(availSeats, date);
	if (pos >= 0)
	{
		if (availSeats[pos].numAvailSeats[1] >= requiredSeats || availSeats[pos].numAvailSeats[2] >= requiredSeats || availSeats[pos].numAvailSeats[3] >= requiredSeats || availSeats[pos].numAvailSeats[4] >= requiredSeats)
			return true;
	}
	return false;
}

bool hasAvailableSeats(const vector< AvailSeats >& availSeats, const Date date,
	int timeCode, int requiredSeats)
{
	int pos = findAvailSeat(availSeats, date);
	if (pos >= 0)
	{
		if (availSeats[pos].numAvailSeats[timeCode] >= requiredSeats)
			return true;
	}
	return false;
}

int findAvailSeat(const vector< AvailSeats >& availSeats, Date date)
{
	size_t num = 0;
	for (size_t i = 0; i < availSeats.size(); i++)
	{
		if (availSeats[i].date.year == date.year && availSeats[i].date.month == date.month && availSeats[i].date.day == date.day)
		{
			return i;
		}
		
	}
	return -1;
}

void inputTimeCode(const vector< AvailSeats >& availSeats, const Date date,
	int& timeCode, int partySize)
{
	char string[3];
	int num = 0;
	int choice;
	for (size_t i = 1; i < 5; i++)
	{
		if (hasAvailableSeats(availSeats, date, i, partySize) == true)
		{
			if (i == 1)
			cout << i << ". 11:30" << endl;
			if (i == 2)
				cout << i << ". 13:30" << endl;
			if (i == 3)
				cout << i << ". 17:45" << endl;
			if (i == 4)
				cout << i << ". 19:45" << endl;
		}
		else
		{
			string[num] = i;
			num++;
		}

	}
	do cout << "? ";
	while ((choice = inputAnInteger(1, 4)) == -1 || choice == string[0] || choice == string[1] || choice == string[2]);
	timeCode = choice;
}

void displayReservationInfo(const Reservation& reservation)
{
	cout << reservation.partySize << " guests  ";

	cout << reservation.date.year << "/";

	if (reservation.date.month < 10)
		cout << '0';
	cout << reservation.date.month << "/";

	if (reservation.date.day < 10)
		cout << '0';
	cout << reservation.date.day;

	char times[5][8] = { "", "11:30", "13:30", "17:45", "19:45" };
	cout << "  " << times[reservation.time] << endl;
}

void decreaseAvailSeats(vector< AvailSeats >& availSeats, const Date date,
	int timeCode, int requiredSeats)
{
	int pos = findAvailSeat(availSeats, date);
	if (pos >= 0) // found
		availSeats[pos].numAvailSeats[timeCode] -= requiredSeats;
}

void viewReservation(const vector< Reservation >& reservations)
{
	if (reservations.size() == 0)
	{
		cout << "\nNo reservations!\n";
		return;
	}

	char phoneNumber[12];
	cout << "\nEnter phone Number: ";
	cin >> phoneNumber;
	cin.ignore();

	if (!exist(reservations, phoneNumber))
	{
		cout << "\nYou have no reservations!\n";
		return;
	}

	displayReservationInfo(reservations, phoneNumber);
}

bool exist(const vector< Reservation >& reservations, char phoneNumber[])
{
	for (size_t i = 0; i < reservations.size(); i++)
	{
		if (strcmp(reservations[i].phoneNumber, phoneNumber) == 0)
			return true;
	}
	return false;
}

void displayReservationInfo(const vector< Reservation >& reservations,
	char phoneNumber[])
{
	cout << endl;
	int count = 0;
	for (size_t i = 0; i < reservations.size(); ++i)
		if (strcmp(reservations[i].phoneNumber, phoneNumber) == 0)
		{
			cout << setw(2) << ++count << ". ";
			displayReservationInfo(reservations[i]);
		}
}

void storeReservations(vector< Reservation >& reservations)
{
	ofstream infile;
	infile.open("Reservations.dat", ios::binary);
	if (!infile)
	{
		cout << "file could not be opened." << endl;
		exit(1);
		system("pause");
	}
	for (size_t i = 0; i < reservations.size(); i++)
	{
		infile.write(reinterpret_cast<char*>(&reservations[i]), sizeof(Reservation));
	}
	infile.close();
}

void storeAvailSeats(vector< AvailSeats >& availSeats)
{
	ofstream infile;
	infile.open("AvailSeats.dat", ios::binary);
	if (!infile)
	{
		cout << "file could not be opened." << endl;
		exit(1);
		system("pause");
	}
	for (size_t i = 0; i < availSeats.size(); i++)
	{
		infile.write(reinterpret_cast<char*>(&availSeats[i]), sizeof(AvailSeats));
	}
	infile.close();

}