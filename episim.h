#ifndef EPICALC_H
#define EPICALC_H

#if (__cplusplus!=1) && (__cplusplus!=199711L)
#include <random>
#endif

class EpiSim
{
public:
    EpiSim(bool prec=false);

    enum CarantineState
    {
        NoCarantine,
        Lockdown,
        FullCarantine
    };

    enum TherapyLevel
    {
        Standart,
        Intense,
        Advanced
    };

    // В сутках - два тика
    void tick();

    double stammContagabilityA; // заразность в активной стадии
    double stammContagabilityP; // заразность в пассивной стадии
    double lethalityCured;          // вероятность летального исхода каждый день при лечении
    double lethalityUnCured;          // вероятность летального исхода каждый день без лечения
    double cureability;        // процент выздоровления каждый день
    double selfCureability;        // процент выздоровления каждый день
    double panic;              // процент паники
    double detectPercent;      // процент обнаружения
    int uninfectedInCity;            // незаражённых в городе
    int infectedPassive;        // заражённых в пассивной стадии
    int infectedActive;         // заражённых в активной стадии
    int infectedTerminal;         // заражённых в терминальной стадии (заразность такая же, как в активной)
    int territory;              // площадь города
    int inHospital;             // в больнице
    int cured;                  // выздоровело
    int died;                   // погибло

    int hospitalCapacity;       // вместительность больниц

    int incomingDensity;    // сколько заражённых приезжает?

    bool isIncomingCarantine; // Карантин приезжих - число появляющихся заражённых рандомно падает.
    bool isThermalChecks; // Проверки температуры - начинает изыматься по 70% заражёных в активной стадии.
    bool isBorderLocked; // Перекрытие границ - заражённые снаружи больше не появляются.
    bool isSomeClosed; // Уменьшение мероприятий - крупные мероприятия ликвидируются, плотность общения понижается на 20%.
    bool isHospitalDoubled;
    bool isHospitalQuadred;
    bool isSchoolClosed; // Закрытие школ - плотность общения понижается ещё на 20%.
    bool isMassClosed; // Отмена массовых мероприятий - плотность общения снижается ещё на 15%.
    bool isDesinfection; // Повсеместная дезинфекция - заразность штамма снижается на 30%.
    CarantineState carantineState;
    //bool isLockDown; // Локдаун (Остановка транспорта) - плотность общения понижается на 50%.
    //bool isFullCarantine; // з) Полный карантин - плотность общения понижается на 50%.
    //bool isIntensiveTherapy; // и) Интенсивная терапия - процент выздоровления повышается на 40%.
    TherapyLevel therapyLevel;
    bool isVirusTests; //к) Тесты на вирус - начинает изыматься ещё по 80% заражённых в пассивной стадии.
    //bool isAdvancedTherapy; // л) Улучшенная терапия - процент выздоровления повышается ещё на 40%.
    bool isVaccine; // м) Вакцина - заразность штамма снижается на 70%.

    int peopleTotal() {return uninfectedInCity+infectedActive+infectedPassive+infectedTerminal;}
    int getHospitalCapability();

    double getCureability();

    bool precise;
#if (__cplusplus!=1) && (__cplusplus!=199711L)
    std::mt19937 gen;
#endif
    double firstSoc;
    double secondSoc;
    double firstEnv;
    double secondEnv;
private:
    double randVal();
    bool roll(double percent);
};

#endif // EPICALC_H
