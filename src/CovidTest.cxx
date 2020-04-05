#include "../sars_cov2_sk/CovidTest.h"
#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/HelperFunctions.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace sars_cov2_sk;

CovidTest::CovidTest(float reliability) {
    m_reliability = reliability;

    m_yesterday = Person::GetDay() - 1;
    m_today     = Person::GetDay();
}

bool CovidTest::Test(Person *person)  const {
    if (!person->IsIll())   {
        return false;
    }
    else    {
        const bool test_result = (RandomUniform() < m_reliability);
        person->SetTestResult(test_result);
        return test_result;
    }
}

int CovidTest::TestPeople(const std::vector<Person *> &persons, float fraction)  {
    if (m_today != Person::GetDay())    {
        m_positively_tested_yesterday = m_positively_tested_today;
        m_positively_tested_today.clear();
        m_today = Person::GetDay();
        m_yesterday = m_today - 1;
    }

    int number_of_positively_tested = 0;
    for (Person *person : persons)  {
        // Do not test healthy people for now
        if (!person->IsIll())   {
            continue;
        }

        // Do not test people who had positive test result already
        if (person->PositivelyTesed())    {
            continue;
        }

        if (person->HealthState() < fraction)    {
            if (Test(person))   {
                number_of_positively_tested++;
                if (IsInVector(m_positively_tested_today, person))  {
                    m_positively_tested_today.push_back(person);
                }
            }
        }
    }
    return number_of_positively_tested;
}

int CovidTest::TestPeople(std::vector<Person> *persons, float fraction) {
    if (m_today != Person::GetDay())    {
        m_positively_tested_yesterday = m_positively_tested_today;
        m_positively_tested_today.clear();
        m_today = Person::GetDay();
        m_yesterday = m_today - 1;
    }

    int number_of_positively_tested = 0;
    for (Person &person : *persons)  {
        // Do not test healthy people for now
        if (!person.IsIll())   {
            continue;
        }

        // Do not test people who had positive test result already
        if (person.PositivelyTesed())    {
            continue;
        }

        if (person.HealthState() < fraction)    {
            if (Test(&person))   {
                number_of_positively_tested++;
                if (!IsInVector(m_positively_tested_today, &person))  {
                    m_positively_tested_today.push_back(&person);
                }
            }
        }
    }
    return number_of_positively_tested;
}

int CovidTest::TestContactOfPositivesFromYesterday()    {
    if (m_today != Person::GetDay())    {
        m_positively_tested_yesterday = m_positively_tested_today;
        m_positively_tested_today.clear();
        m_today = Person::GetDay();
        m_yesterday = m_today - 1;
    }

    int number_of_positively_tested = 0;
    for (Person *person : m_positively_tested_yesterday)  {
        // Do not test healthy people for now
        if (!person->IsIll())   {
            continue;
        }

        const vector<Person *> *list_if_contacts = person->GetListOfContacts();
        for (Person *contact : *list_if_contacts)   {
            // Do not test people who had positive test result already
            if (contact->PositivelyTesed())    {
                continue;
            }

            if (Test(contact))   {
                number_of_positively_tested++;
                if (!IsInVector(m_positively_tested_today, contact))  {
                    m_positively_tested_today.push_back(contact);
                }
            }
        }
    }
    return number_of_positively_tested;
}

void CovidTest::PutToCarantinePositivelyTestedFromYesterday()   {
    for (Person *person : m_positively_tested_yesterday)  {
        person->PutToQuarantine();
    }
}