#include <iostream>
#include <vector>
#include <numeric>

#include "Day.h"
#include "Meal.h"

Day::Day()
{
}

void Day::GetStatistics()
{
    //TODO: add buton in table to display full day's statistic
    int dayNumber = 1;
    for (Meal& meal : mConsumedMeals)
    {
        std::cout << "Day: " << dayNumber << std::endl;
        meal.GetMealInfo();
    }
}

int Day::GetConsumedProteins() const
{
    //We call it evry frame, it might be not very well.
    return std::accumulate(mConsumedMeals.begin(), mConsumedMeals.end(), 0 , [](int sum, Meal meal)
    {
         return sum += meal.GetProteins();
    });
}

int Day::GetConsumedFats() const
{
    //TODO: maybe utils class for calculation?
    return std::accumulate(mConsumedMeals.begin(), mConsumedMeals.end(), 0 , [](int sum, Meal meal)
    {
         return sum += meal.GetFats();
    });
}

int Day::GetConsumedCarbohydrates() const
{
    return std::accumulate(mConsumedMeals.begin(), mConsumedMeals.end(), 0 , [](int sum, Meal meal)
    {
         return sum += meal.GetCarbohydrates();
    });
}

int Day::GetTotalCalories() const
{
    return std::accumulate(mConsumedMeals.begin(), mConsumedMeals.end(), 0, [](int sum, Meal meal)
    {
        return sum += meal.GetTotalCalories();
    });
}

int Day::GetMealCout() const
{
    return static_cast<int>(mConsumedMeals.size());
}

void Day::AddMeal(int proteins, int carbohydrates, int fats)
{
    mConsumedMeals.push_back(Meal(proteins, carbohydrates, fats));
}

std::string Day::GetConsumedInfoShort()
{
    int totalCalories = GetTotalCalories();
    int carbohydrates = GetConsumedCarbohydrates();
    int proteins = GetConsumedProteins();
    int fats = GetConsumedFats();
    char buff[256];
    snprintf(buff, sizeof(buff), "Consumed callories: %d carbohydrates: %d proteins: %d fats: %d", totalCalories, carbohydrates, proteins, fats);
    std::string info = buff;

    return info;
}
