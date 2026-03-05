#ifndef SHOPSYSTEM_H
#define SHOPSYSTEM_H

#include <vector>
#include <iostream>
#include "ModifierFactory.h"

class ShopSystem {
public:

    static void openShop(std::vector<IModifier*>& inventory, int& currency) {

        while (true) {

            std::cout << "\n===== SHOP =====\n";
            std::cout << "Currency: $" << currency << "\n";
            std::cout << "1. Lanjut sesi berikutnya\n";
            std::cout << "2. Beli\n";
            std::cout << "3. Jual\n";
            std::cout << "4. Exit to Menu\n";
            std::cout << "Pilih: ";

            std::string input;
            getline(std::cin,input);

            // ===== LANJUT =====
            if(input=="1"){
                return;
            }

            // ===== BELI =====
            else if(input=="2"){

                while(true){

                    std::cout<<"\n=== BELI MODIFIER ===\n";
                    std::cout<<"1. 2x mult skor ($6)\n";
                    std::cout<<"2. +50 skor ($4)\n";
                    std::cout<<"3. Back\n";
                    std::cout<<"Pilih: ";

                    getline(std::cin,input);

                    if(input=="1"){

                        if(currency>=6){

                            currency-=6;

                            inventory.push_back(
                                ModifierFactory::createModifier("double")
                            );

                            std::cout<<"Berhasil membeli 2x mult skor\n";
                        }
                        else{
                            std::cout<<"Currency tidak cukup\n";
                        }
                    }

                    else if(input=="2"){

                        if(currency>=4){

                            currency-=4;

                            inventory.push_back(
                                ModifierFactory::createModifier("flat")
                            );

                            std::cout<<"Berhasil membeli +50 skor\n";
                        }
                        else{
                            std::cout<<"Currency tidak cukup\n";
                        }
                    }

                    else if(input=="3"){
                        break;
                    }

                    else{
                        std::cout<<"Pilihan tidak valid\n";
                    }
                }
            }

            // ===== JUAL =====
            else if(input=="3"){

                if(inventory.empty()){
                    std::cout<<"Tidak ada modifier untuk dijual\n";
                    continue;
                }

                std::cout<<"\n=== INVENTORY ===\n";

                for(size_t i=0;i<inventory.size();i++){
                    std::cout<<i+1<<". "<<inventory[i]->getName()<<"\n";
                }

                std::cout<<"Pilih modifier yang ingin dijual (0 untuk cancel): ";

                getline(std::cin,input);

                int idx = stoi(input);

                if(idx<=0 || idx>inventory.size())
                    continue;

                IModifier* mod = inventory[idx-1];

                if(mod->getName().find("Double")!=std::string::npos)
                    currency+=3;
                else
                    currency+=2;

                delete mod;

                inventory.erase(inventory.begin()+idx-1);

                std::cout<<"Modifier berhasil dijual\n";
            }

            // ===== EXIT MENU =====
            else if(input=="4"){
                throw std::runtime_error("exit");
            }

            else{
                std::cout<<"Pilihan tidak valid\n";
            }
        }
    }
};

#endif