#ifndef RESTRICTIONS_H
#define RESTRICTIONS_H

#include "../sars_cov2_sk/Person.h"

#include<vector>
#include<string>
#include<map>

namespace sars_cov2_sk	{
    // singleton
    enum SmartResctrictionsType {   enum_smart_restrictions_none,
                                    enum_smart_restrictions_critical,
                                    enum_smart_restrictions_positive_tests,
                                    enum_smart_restrictions_hospitalized_all};
    class Restrictions  {
        private:
            Restrictions(const std::string &resctrictions_file);

            static sars_cov2_sk::Restrictions *s_singleton_instance;

            float m_today_limit_mobility;
            float m_today_limit_elderly_stochastic_interactions;
            float m_today_limit_stochastic_interactions;

            std::vector<float> m_limit_mobility;
            std::vector<float> m_limit_elderly_stochastic_interactions;
            std::vector<float> m_limit_stochastic_interactions;

            // smart restrictions
            SmartResctrictionsType  m_smart_restrictions_type;
            bool                    m_smart_restrictions_active;
            int                     m_trigger_count_on;
            int                     m_trigger_count_off;
            float                   m_smart_limit_mobility;
            float                   m_smart_limit_elderly_stochastic_interactions;
            float                   m_smart_limit_stochastic_interactions;


            const std::vector<sars_cov2_sk::Person> *m_population;

            std::map<std::string, float> *ReadStringFloatDictionary(const std::string &json_file, const std::string &dict_name);

            void InitializeInteligentRestrictions(const std::string &resctrictions_file);

            void SetValuesForInteligentRestrictions(std::map<std::string, float> *restrictions_dictionary);

            // it counts the number of people in the state that triggers the smart restrictions
            // if the number of people is hight enough, it put restrictions in place
            // if the restrictions are in place and the number of people in the given state descreases bellow the given threshold, it cancels the restrictions
            void CheckNeedForSmartRestrictions();

        public:

            static void Initialize();

            static void Check();

            static void SetPopulationPointer(const std::vector<sars_cov2_sk::Person> *population)   {Check(); s_singleton_instance->m_population = population;};

            static void UpdateDay(int day);

            static float GetLimitMobility()                         {Check(); return s_singleton_instance->m_today_limit_mobility;};

            static float GetLimitElderlyStochasticInteractions()    {Check(); return s_singleton_instance->m_today_limit_elderly_stochastic_interactions;};

            static float GetLimitStochasticInteractions()           {Check(); return s_singleton_instance->m_today_limit_stochastic_interactions;};

    };
};
#endif
