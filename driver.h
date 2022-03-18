/*
 * drive.h -> contains structures of question and person which will be used to
 * store data from input files. Also contains functions to choose random
 * questions, isolate, normalize and compare OCEAN scores.
 *
 */

#include <math.h>

#include <limits>
#include <map>
#include <set>
#include <string>

#include "myrandom.h"

using namespace std;

constexpr double lowest_double = std::numeric_limits<double>::lowest();

/* Type: Question
 *
 * Type representing a personality quiz question.
 */
struct Question {
  string questionText;     // Text of the question
  map<char, int> factors;  // Map from factors to +1 or -1
  friend bool operator<(const Question& lhs, const Question& rhs) {
    return lhs.questionText < rhs.questionText;
  }
  friend bool operator==(const Question& lhs, const Question& rhs) {
    return lhs.questionText == rhs.questionText;
  }
  friend bool operator!=(const Question& lhs, const Question& rhs) {
    return lhs.questionText != rhs.questionText;
  }
};

/* Type: Person
 *
 * Type representing a person, used to represent people when determining
 * who's the closest match to the user.
 */
struct Person {
  string name;            // Name of the person
  map<char, int> scores;  // Map from factors to +1 or -1
  friend bool operator<(const Person& lhs, const Person& rhs) {
    return lhs.name < rhs.name;
  }
  friend bool operator==(const Person& lhs, const Person& rhs) {
    return lhs.name == rhs.name;
  }
  friend bool operator!=(const Person& lhs, const Person& rhs) {
    return lhs.name != rhs.name;
  }
};

/* randomElement
 *
 * This function selects, at random, a Question from the inputted questions set
 * and returns the question.  Note, this function does not remove the randomly
 * selected question from the set.
 */
Question randomElement(set<Question>& questions) {
  int ind = randomInteger(0, (int)questions.size() - 1);
  int i = 0;
  for (auto e : questions) {
    if (i == ind) {
      return e;
    }
    i++;
  }
  return {};
}

/* randomQuestionFrom (MS1)
 *
 * Uses randomElement function to select a random question, then erases this
 * question from the set of questions. function returns random question.
 */
Question randomQuestionFrom(set<Question>& questions) {
  // error handling
  if (questions.empty()) {
    throw runtime_error("Question set is empty!");
  }

  Question thisQuestion;
  thisQuestion = randomElement(questions);
  questions.erase(thisQuestion);

  return thisQuestion;
}

/* scoresFrom (MS2)
 *
 * Given a map if questions and the recorded answers of these questions, this
 * function calculates the inpact of the answer the the specific question it,
 * was given to. This function is O(n) since it iterates through the list of
 * answers. the second iteration iterates through the posible ocean score
 * which could be at most 5 for each n.
 */
map<char, int> scoresFrom(map<Question, int>& answers) {
  map<char, int> oceanScores;

  for (const auto& currAns : answers) {
    for (const auto& currQuestion : currAns.first.factors) {
      oceanScores[currQuestion.first] =
          oceanScores[currQuestion.first] +
          (currQuestion.second * (currAns.second - 3));
    }
  }

  return oceanScores;
}

/* normalize (MS3)
 *
 * Given a map of OCEAN scores, the normalize function will scale the OCEAN
 * scores, this will allow us to compare the values with other sets of OCEAN
 * scores.
 */
map<char, double> normalize(map<char, int>& scores) {
  double powerSum = 0;
  double euclidNorm;
  map<char, double> normalizedScores;

  // finds the sum of all elements squared.
  for (const auto& currScore : scores) {
    powerSum = powerSum + pow(currScore.second, 2);
  }

  if (powerSum == 0) {
    throw runtime_error("map is empty!");
  }

  // Euclidean norm: the square root of the sum of all the squares.
  euclidNorm = sqrt(powerSum);

  // divide each score by Euclidean norm to normalize.
  for (const auto& currScore : scores)
    normalizedScores[currScore.first] = (currScore.second / euclidNorm);

  return normalizedScores;
}

/* cosineSimilarityOf (MS4)
 *
 * Given two normailized maps of OCEAN scores, this function finds how similar
 * these two maps are. This is done by adding all the products of the OCEAN
 * scores. This results in a value of -1 to 1, -1 being completely dissimilar
 * and 1 being compleately similar.
 */
double cosineSimilarityOf(const map<char, double>& lhs,
                          const map<char, double>& rhs) {
  double similarity = 0;

  for (const auto& lhsScores : lhs) {
    if (rhs.count(lhsScores.first) > 0) {
      similarity = similarity + (lhsScores.second * rhs.at(lhsScores.first));
    }
  }
  return similarity;
}

/* mostSimilarTo (MS5)
 *
 * This function finds, from a set of 'people' with associated OCEAN scores,
 * the most similar person to the given answers to questions. returns the
 * most similar 'person'
 */
Person mostSimilarTo(map<char, int>& scores, set<Person>& people) {
  Person similarPerson;
  string personKey;
  double tempSimilarity;
  double largestSimilarity = -2.0;

  if (people.empty()) throw runtime_error("People set is empty!");
  if (scores.empty()) throw runtime_error("Scores map is empty!");

  map<char, double> finalScores = normalize(scores);

  for (auto p : people) {
    tempSimilarity = cosineSimilarityOf(finalScores, normalize(p.scores));
    if (tempSimilarity > largestSimilarity) {
      largestSimilarity = tempSimilarity;
      similarPerson = p;
    }
  }

  return similarPerson;
}