#pragma once

//TODO: namcespace as well
class Meal
{
public:
    Meal(int proteins, int carbohydrates, int fats);

    //TODO: maybe rename
    void GetMealInfo() const;
    int GetProteins() const;
    int GetFats() const;
    int GetCarbohydrates() const;
    int GetTotalCalories() const;

private:
    //TODO: replace to utils namespace??
    static const int PROTEIN_CALORIES = 4;
    static const int CARBOHYDRATES_CALORIES = 4;
    static const int FATS_CALORIES = 9;
    
    int mProteins;
    int mCarbohydrates;
    int mFats;
};

