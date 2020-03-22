#include "episim.h"
#include <stdlib.h>
#include <QtGlobal>
#include <QDebug>

bool EpiSim::roll(double percent)
{
    double val ;

#if (__cplusplus==1) || (__cplusplus==199711L)
    val = double(qrand())/double(RAND_MAX);
#else
    if (precise)
        val = std::generate_canonical<double, 1>(gen);
    else
        val = double(qrand())/double(RAND_MAX);
#endif

    if (val<percent) return true;
    else return false;
}

double EpiSim::randVal()
{
#if (__cplusplus==1) || (__cplusplus==199711L)
    return double(qrand())/double(RAND_MAX);
#else
    if (precise)
        return std::generate_canonical<double, 1>(gen);
    else
        return double(qrand())/double(RAND_MAX);
#endif
}



EpiSim::EpiSim(bool prec)
{
    precise = prec;

    std::srand(std::time(0)); //Используем текущее время, как начальное значение для генератора случайных чисел


#if (__cplusplus!=1) && (__cplusplus!=199711L)
    std::random_device rd;
    gen = std::mt19937(rd());
#endif

    stammContagabilityA = 0.5; // заразность в активной стадии
    stammContagabilityP = 0.05; // заразность в пассивной стадии
    lethalityCured = 0.01;          // летальность
    selfCureability = 0.01;
    lethalityUnCured = 0.2;         // без лечения
    cureability = 0.1;        // процент выздоровления
    panic = 0; //.3;              // процент паники
    detectPercent = 0.3;      // процент обнаружения
    uninfectedInCity = 100000;            // всего в городе
    infectedPassive = 0;        // заражённых в пассивной стадии
    infectedActive = 0;         // заражённых в активной стадии
    infectedTerminal = 0;         // заражённых в терминальной стадии
    territory = 80000;              // площадь города
    inHospital = 0;             // в больнице
    cured = 0;                  // выздоровело
    died = 0;                   // погибло
    hospitalCapacity = 2000;
    carantineState = NoCarantine;
    therapyLevel = Standart;

    isIncomingCarantine = false;
    isThermalChecks = false;
    isBorderLocked = false;
    isSomeClosed = false;
    isHospitalDoubled = false;
    isHospitalQuadred = false;
    isSchoolClosed = false;
    isMassClosed = false;
    isDesinfection = false;
    isVirusTests = false;
    isVaccine = false;

    incomingDensity = 3;        // сколько приезжает заражённых?
}

void EpiSim::tick()
{
    if (!isBorderLocked)
        infectedPassive += incomingDensity - isIncomingCarantine * randVal();

    // Гибель и лечение
    int curing =0;
    for(int i=0; i<inHospital; i++)
        if (roll(getCureability()))
            curing++;

    inHospital-=curing;
    uninfectedInCity+=curing;
    cured+=curing;

    int dieingHospital = 0;
    for(int i=0; i<inHospital; i++)
        if (roll(lethalityCured))
            dieingHospital++;

    inHospital-=dieingHospital;
    died += dieingHospital;

    // Гибель больных в терминальной стадии
    int dieingTerminal = 0;
    for(int i=0; i<infectedTerminal; i++)
        if (roll(lethalityUnCured))
            dieingTerminal++;

    infectedTerminal-=dieingTerminal;
    died += dieingTerminal;

    // Проверка перехода из пассивной стадии в активную -
    // пусть будет с той же вероятностью, что и заразность

    int pToA=0;
    for(int i=0; i<infectedPassive; i++)
        if (roll(stammContagabilityP))
            pToA++;

    infectedPassive -= pToA;
    infectedActive += pToA;

    firstSoc=0; secondSoc=0; firstEnv=0; secondEnv=0;

    // Проверка заражения. За тик будет по 2 попытки заразить других людей
    int totalNewInfected=0;
    for(int k=0; k<2; k++)
    {
        // Сначала найдём среднюю "общительность" населения
        double communicability = randVal();
        if (isSomeClosed) communicability *= 0.8;
        if (isSchoolClosed) communicability *= 0.8;
        if (isMassClosed) communicability *= 0.9;
        if (carantineState==Lockdown) communicability *= 0.65;
        if (carantineState==FullCarantine) communicability *= 0.3;

        double density =  peopleTotal() / double(territory);
        // Фактор окружения - плотность населения,
        // некоторая случайная "плотность общения" населения
        // и показатель паники (чем он больше, тем выше вероятность заражения).
        double envirFactor = density * communicability * (1.0+panic);
        if (isDesinfection) envirFactor *= 0.8;

        double contagabilityP = stammContagabilityP * envirFactor;
        double contagabilityA = stammContagabilityA * envirFactor;
        if (isVaccine)
        {
            contagabilityP *= 0.4;
            contagabilityA *= 0.4;
        }

        if (firstSoc<=0.000001) {  firstSoc = communicability; firstEnv = envirFactor; }
        else {  secondSoc = communicability; secondEnv = envirFactor; }

        int newInfected = 0;
        for(int i=0; i<infectedPassive; i++)
            if (roll(contagabilityP))
                newInfected++;

        for(int i=0; i<infectedActive+infectedTerminal; i++)
            if (roll(contagabilityA))
                newInfected++;

        if (uninfectedInCity<newInfected) newInfected = uninfectedInCity;

        totalNewInfected+=newInfected;

        uninfectedInCity -= newInfected;
        infectedPassive += newInfected;
    }

    // Теперь проверка обнаружения.
    // Считаем, что всех "терминальных" знают уже СРАЗУ и пытаются разместить
    // в госпитали. Пассивные не обнаруживаются, а активные - с некоторым шансом
    // Если все не уместятся в госпитале - часть болеющих сядут дома, то есть станут пассивны
    inHospital += infectedTerminal;
    infectedTerminal = 0;
    if (inHospital>getHospitalCapability())
    {
        int notInHospital = inHospital - getHospitalCapability();
        infectedTerminal += notInHospital;
        inHospital = getHospitalCapability();
    }

    // Обнаруживаем активных
    int detected = 0;
    for(int i=0; i<infectedActive; i++)
        if (roll(detectPercent * (1+0.35*isThermalChecks)))
            detected++;

    infectedActive-=detected;
    inHospital += detected;

    // Если все не уместятся в госпитале - часть болеющих сядут дома, то есть станут пассивны
    if (inHospital>getHospitalCapability())
    {
        int notInHospital = inHospital - getHospitalCapability();
        infectedPassive += notInHospital;
        inHospital = getHospitalCapability();
    }

    // Если есть тесты на вирус - то проверим пассивных
    if (isVirusTests)
    {
        int detectedPassive = 0;

        for(int i=0; i<infectedPassive; i++)
            if (roll(detectPercent))
                detectedPassive++;

        infectedPassive-=detectedPassive;
        inHospital += detectedPassive;

        // Если все не уместятся в госпитале - часть болеющих сядут дома, то есть станут пассивны
        if (inHospital>getHospitalCapability())
        {
            int notInHospital = inHospital - getHospitalCapability();
            infectedPassive += notInHospital;
            inHospital = getHospitalCapability();
        }
    }

    // Переход из активной стадии в терминальную
    int aToT=0;
    for(int i=0; i<infectedActive; i++)
        if (roll(stammContagabilityP))
            aToT++;

    infectedActive -= aToT;
    infectedTerminal += aToT;

    // Есть и шанс самостоятельного выздоровления - невысокий!
    int selfCureP=0;
    for(int i=0; i<infectedPassive; i++)
        if (roll(selfCureability))
            selfCureP++;

    infectedPassive -= selfCureP;
    uninfectedInCity += selfCureP;

    int selfCureA=0;
    for(int i=0; i<infectedActive; i++)
        if (roll(selfCureability))
            selfCureA++;

    infectedActive -= selfCureA;
    uninfectedInCity += selfCureA;


    int selfCureT=0;
    for(int i=0; i<infectedTerminal; i++)
        if (roll(selfCureability))
            selfCureT++;

    infectedTerminal -= selfCureT;
    uninfectedInCity += selfCureT;

    cured += selfCureA+selfCureP+selfCureT;

    // Отчёт
    /*
    qDebug() << "Разносчиков: " << infectedActive+infectedTerminal << ", инкубац. период: " << infectedPassive;
    qDebug() << "Здоровых:" << uninfectedInCity ;
    qDebug() << "А) Соц. активность:" <<  QString::number(firstSoc, 'f', 2) << ", заразность среды:" << QString::number(firstEnv, 'f', 5);
    qDebug() << "Б) Соц. активность:" <<  QString::number(secondSoc, 'f', 2) << ", заразность среды:" << QString::number(secondEnv, 'f', 5);
    qDebug() << "Новые случаи:" << totalNewInfected << ", умерло:" << dieingHospital+dieingTerminal ;
    qDebug() << "В больнице:" << inHospital << ", при смерти:" << infectedTerminal;
    qDebug() << "";
    */
}

int EpiSim::getHospitalCapability()
{
    return hospitalCapacity*(1+isHospitalDoubled)*(1+isHospitalQuadred);
}

double EpiSim::getCureability()
{
    switch (therapyLevel) {
    case Standart:
        return cureability;
        break;
    case Intense:
        return cureability*1.4;
        break;
    case Advanced:
        return cureability*1.4 * 1.4;
        break;
    default:
        break;
    }
    return cureability;
}
