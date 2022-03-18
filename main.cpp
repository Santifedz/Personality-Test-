/*
 * main.cpp
 * takes input files containing a variety of questions with their respective
 * OCEAN score impact, then asks the user how many questions they want to aswer.
 * once these answers are given, utilizes functions from driver.h to compare and
 * give the most similar 'person' from a selected group of people.
 * Created by Santiago Fernandez.
 */

#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "driver.h"

using namespace std;

// openF() checks if the given file is usable.
int openF(ifstream& Input, string Fname) {
  Input.open(Fname);

  if (!Input.is_open() | Input.fail()) {
    cout << "Error: unable to open '" << Fname << "'\n";
    return 1;
  } else
    return 0;
}

// sinqData() helper function, stores input into a given structure by referance.
void sinqData(ifstream& Input, string& thisString, map<char, int>& thisMap) {
  string tempString;
  int OCEANint;
  char OCEANchar;

  getline(Input, tempString);
  stringstream tempSS(tempString);
  getline(tempSS, thisString, '.');

  while (tempSS.eof() != true) {
    tempSS.ignore(1);
    tempSS.get(OCEANchar);
    tempSS.ignore(1);
    tempSS >> OCEANint;
    thisMap[OCEANchar] = OCEANint;
  }
}

// loadQuestions() iterated through questions.txt and calling sinqData for each
// line in questions.txt. This retrieves one question from the file then adds
// this question to a set of question structures. Ifstream is closed at the end
// of this function.
void loadQuestions(set<Question>& qs) {
  ifstream Input;
  openF(Input, "questions.txt");

  for (int i = 0; i < 486; i++) {
    Question q;
    sinqData(Input, q.questionText, q.factors);
    qs.insert(q);
  }

  Input.close();
}

// askQs() asked the user how many questions they want to answer, then retieves
// that number of random questions(using randomQuestionFrom from drive.h) from
// a set of Question structures. Then, it records an asnwer from the user and
// stores it in a map of key:question, value: integers that represent the
// asnwer of said question.
void askQs(int qnum, set<Question>& qs, map<Question, int>& Answers) {
  int answer = 0;

  for (int i = 0; i < qnum; i++) {
    Question thisQuestion = randomQuestionFrom(qs);
    cout << "\nHow much do you agree with this statement?\n";
    cout << '"' << thisQuestion.questionText << '.' << '"' << endl;

    cout << "\n1. Strongly disagree";
    cout << "\n2. Disagree";
    cout << "\n3. Neutral";
    cout << "\n4. Agree";
    cout << "\n5. Strongly agree\n";

    if (answer < 1 || answer > 5) {
      cout << "\nEnter your answer here (1-5): ";
      cin >> answer;
    }

    Answers[thisQuestion] = answer;
    answer = 0;
  }
}

// loadPeople() iterates through the .people file selected and uses sinqData()
// to store one line at a time the data from said file into People structres
// and add these people structures to a set of People.
void loadPeople(string Fname, set<Person>& prs) {
  ifstream Input;
  openF(Input, Fname);

  while (Input.eof() != true) {
    Person p;
    sinqData(Input, p.name, p.scores);
    // only adds a person if line is not empty.
    if (p.name != "") prs.insert(p);
  }
  Input.close();
}

// getFname() helper fuction contains a switch staatement that retreives a file
// name depending on what case is given
string getFname(int choice) {
  switch (choice) {
    case 1:
      return "BabyAnimals.people";
    case 2:
      return "Brooklyn99.people";
    case 3:
      return "Disney.people";
    case 4:
      return "Hogwarts.people";
    case 5:
      return "MyersBriggs.people";
    case 6:
      return "SesameStreet.people";
    case 7:
      return "StarWars.people";
    case 8:
      return "Vegetables.people";
    case 9:
      return "cereal.people";
    default:
      return " ";
  }
}

// comparePeople() prompts user to choose what ser of 'People' they want to be
// compared to. then depending on their selection, a set of People is loaded
// using loadPeopl() and a set of answers is loaded using scoresFrom(). These
// two sets are then compared using mostSimilarTo().
void comparePeople(map<Question, int>& answers) {
  bool runBool = true;
  int choice;
  map<char, int> scores;
  set<Person> peopleSet;

  while (runBool == true) {
    cout << "\n1. BabyAnimals";
    cout << "\n2. Brooklyn99";
    cout << "\n3. Disney";
    cout << "\n4. Hogwarts";
    cout << "\n5. MyersBriggs";
    cout << "\n6. SesameStreet";
    cout << "\n7. StarWars";
    cout << "\n8. Vegetables";
    cout << "\n9. mine";  //(MS7) cereal.people
    cout << "\n0. To end program.\n";
    cout << "\nChoose test number (1-9, or 0 to end): ";

    cin >> choice;

    if (choice == 0) {
      runBool = false;
      cout << "Goodbye!\n";
      break;
    }

    scores = scoresFrom(answers);
    loadPeople(getFname(choice), peopleSet);
    cout << "\nYou got " << mostSimilarTo(scores, peopleSet).name << "!\n";

    // this function will continue to compare sets of answers to prompted
    // selection of people until prompted to stop. The set of people is cleared
    // each time.
    peopleSet.clear();
  }
}

// main() calls above functions (MS6)
int main() {
  size_t qnum = 488;
  set<Question> Qset;

  map<Question, int> Answers;
  loadQuestions(Qset);

  if (qnum > Qset.size() || qnum < 1) {
    cout << "Welcome to the Personality Quiz!\n";
    cout << "\nChoose number of questions: ";
    cin >> qnum;
  }

  askQs(qnum, Qset, Answers);

  comparePeople(Answers);

  return 0;
}
