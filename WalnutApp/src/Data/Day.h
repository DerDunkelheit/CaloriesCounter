#pragma once

#include <vector>
#include <string>

#include "Data/Meal.h"

// TODO: create a namespace
class Day
{
public:
    Day();

    void GetStatistics();

    int GetConsumedProteins() const;
    int GetConsumedFats() const;
    int GetConsumedCarbohydrates() const;
    int GetTotalCalories() const;
    int GetMealCout() const;
    void AddMeal(int proteins, int carbohydrates, int fats);
    std::string GetConsumedInfoShort();
private:
    std::vector<Meal> mConsumedMeals;
};

