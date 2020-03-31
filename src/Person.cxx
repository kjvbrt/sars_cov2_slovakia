#include "../sars_cov2_sk/Person.h"
#include "../sars_cov2_sk/HelperFunctions.h"
#include "../sars_cov2_sk/ConfigParser.h"

#include <vector>
#include <stdlib.h>
#include <iostream>

using namespace std;
using namespace sars_cov2_sk;

int Person::s_day_index = 0;

Person::Person() {
    m_health_state      = RandomUniform();
    m_is_ill            = false;
    m_has_symptoms      = false;
    m_is_immune          = false;
    m_is_infective      = false;
    m_in_quarantine     = false;
    m_is_dead           = false;
    m_needs_hospitalization = false;
    m_is_hospitalized   = false; 
    m_day_of_infection  = -1;

    m_infective_period  = RandomGauss(ConfigParser::GetInfectiousDaysMean(),ConfigParser::GetInfectiousDaysStd());
}

void Person::Infect()   {
    if (!m_is_immune && !m_is_ill)    {
        m_is_ill            = true;
        m_day_of_infection  = s_day_index;
    }
}
void Person::AddContact(const sars_cov2_sk::Person *person)   {
    if (ConfigParser::GetTrackingOption() == disabled) return;

    for (const Person *p : m_list_of_contacts){
        if (p == person) {
            return; // Do not add the same contact twice
        }
    }
    m_list_of_contacts.push_back(person);
}

void Person::Kill()     {
    m_is_ill            = false;
    m_has_symptoms      = false;
    m_is_immune          = true;
    m_is_infective      = false;
    m_in_quarantine     = true;
    m_is_dead           = true;
    m_needs_hospitalization = false;
    m_is_hospitalized   = false; 
    m_day_of_infection  = -1;
}

void Person::Heal() {
    m_is_ill            = false;
    m_has_symptoms      = false;
    m_is_immune          = true;
    m_is_infective      = false;
    m_needs_hospitalization = false;
    m_is_hospitalized   = false; 
    m_day_of_infection  = -1;

    ReleseFromToQuarantine();

    // Temporary fix in order to save memory and speed up the code
    ForgetContacts();
}

// #TODO: Check if there is an available bed in hospital
void Person::Hospitalize() {
    m_is_hospitalized = true;
    PutToQuarantine();
}

void Person::PutToQuarantine()  {
    m_in_quarantine = true;
}

void Person::ReleseFromToQuarantine()   {
    m_in_quarantine = false;
};

// #TODO: Improve the method (for now everything is deterministic)
void Person::Evolve()   {
    if (!m_is_ill || m_is_dead) {
        return;
    }

    const int length_of_infection = s_day_index - m_day_of_infection;

    if (length_of_infection == 5)   {
        m_is_infective = true;
    }
    if (length_of_infection == 5+m_infective_period)    {
        Heal();
    }
};

bool Person::IsNewCase()        const   {
    return (m_is_ill && (s_day_index == m_day_of_infection));
};

void Person::ForgetContacts()   {
    m_list_of_contacts.clear();
    m_list_of_contacts.shrink_to_fit();
};

void Person::SetDay(int day) {
    s_day_index = day;
};

void Person::Meet(Person *person1, Person *person2, float transmission_probability, float probability_to_remember)   {
    // You can't meet yourself. But this situation might happen in the code, although it's very unlikely
    if (person1 == person2) {
        return;
    }

    // If one of the persons is in quarantine, the meeting cannot happen
    if (person1->InQuarantine() || person2->InQuarantine()) {
        return;
    }

    // Do nothing if nobody is infected
    if (!person1->IsIll() && !person2->IsIll()) {
        return;
    }

    // We do not remember all people we met ...
    const bool remember = RandomUniform() < probability_to_remember;

    // if both of them are infected, just keep track of the contact
    if (person1->IsIll() && person2->IsIll())   {
        if (remember) person1->AddContact(person2);
        if (remember) person2->AddContact(person1);
        return;
    }

    // If person is infected, we have to keep track of its contacts and spread the virus
    if (person1->IsIll() && !person2->IsIll())   {
        const bool spread_virus = RandomUniform() < transmission_probability;
        if (spread_virus)   {
            if (person1->IsInfective())    person2->Infect();
            if (remember) person2->AddContact(person1);
        }
        if (spread_virus || ConfigParser::GetTrackingOption() == all)   {
            if (remember) person1->AddContact(person2);
        }
        return;
    }

    // If person is infected, we have to keep track of its contacts and spread the virus
    if (!person1->IsIll() && person2->IsIll())   {
        const bool spread_virus = RandomUniform() < transmission_probability;
        if (spread_virus)   {
            if (person2->IsInfective())    person1->Infect();
            if (remember) person1->AddContact(person1);
        }

        // if tracking == all, save the contact, other options are considered by AddContact() method itself
        if (spread_virus || ConfigParser::GetTrackingOption() == all)   {
            if (remember) person2->AddContact(person2);
        }
        return;
    }
};


int Person::GetNumberOfInfectedPersonsInPopulation(const vector<Person *> &population) {
    int infected = 0;
    for (Person *person : population) {
        if (person->IsIll())    {
            infected++;
        }
    }
    return infected;
};

int Person::GetNumberOfInfectedPersonsInPopulation(const vector<Person> &population){
    int infected = 0;
    for (const Person &person : population) {
        if (person.IsIll())    {
            infected++;
        }
    }
    return infected;
};