#ifndef MODIFIERFACTORY_H
#define MODIFIERFACTORY_H

#include "modifiers/IModifiers.h"
#include "modifiers/ModifiersA.h"
#include "modifiers/ModifiersB.h"

class ModifierFactory {
public:

    static IModifier* createModifier(const std::string& type){

        if(type=="double")
            return new DoubleMultiplierModifier();

        if(type=="flat")
            return new FlatBonusModifier();

        return nullptr;
    }
};

#endif