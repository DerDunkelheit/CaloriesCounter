#include <iostream>

#include "Meal.h"

Meal::Meal(int proteins, int carbohydrates, int fats)
    : mProteins(proteins)
    , mCarbohydrates(carbohydrates)
    , mFats(fats)
{

}

void Meal::GetMealInfo() const
{
    using namespace std;
    //TODO: display info not in console but in GUI, probably we can use Strategy for that. just return string and use it in GUI

    cout << "proteins: " << mProteins << "carbohydrates: " << mCarbohydrates << "fats: " << mFats << endl;
}

int Meal::GetProteins() const
{
    return mProteins;
}

int Meal::GetFats() const
{
    return mFats;
}

int Meal::GetCarbohydrates() const
{
    return mCarbohydrates;
}

int Meal::GetTotalCalories() const
{
    return mProteins * PROTEIN_CALORIES + mCarbohydrates * CARBOHYDRATES_CALORIES + mFats * FATS_CALORIES;
}
