#include "RunSession.h"
#include "ScoringSystem.h"
#include "ModifierFactory.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>

using namespace std;

void RunSession::startMenu() {
    bool isRunning = true;
    string choice;
    while (isRunning) {
        cout << "\n==============================\n";
        cout << "    CARD RUN GAME PROTOTYPE   \n";
        cout << "==============================\n";
        cout << "1. Start\n2. Exit\nPilih menu (1/2): ";
        getline(cin, choice);
        if (choice == "1") runGameLoop();
        else if (choice == "2") isRunning = false;
        else cout << "Pilihan tidak valid.\n";
    }
}

void RunSession::initializeDeck() {
    deck.clear();

    string bentuk[] = {"Spades", "Hearts", "Diamonds", "Clubs"};
    string nomor[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
    int skorKartu[] = {2,3,4,5,6,7,8,9,10,10,10,10,11};

    for (int b=0;b<4;++b) {
        for (int n=0;n<13;++n) {
            deck.push_back(Card(nomor[n],bentuk[b],skorKartu[n]));
        }
    }

    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    shuffle(deck.begin(),deck.end(),default_random_engine(seed));
}

void RunSession::runGameLoop() {

    cout << "\n=== PERMAINAN DIMULAI ===\n";

    ScoringSystem scorer;
    scorer.setStrategy(new StandardPokerStrategy());

    int currency = 0;

    const int totalRonde = 5;
    bool gameOver = false;
    bool exitToMenu = false;

    for (int ronde=1; ronde<=totalRonde && !gameOver && !exitToMenu; ++ronde) {

        cout << "\n==============================\n";
        cout << "          RONDE " << ronde << "\n";
        cout << "==============================\n";

        int goalSmall = 25 + (ronde-1)*5;
        int goalHigh  = 50 + (ronde-1)*5;
        int goalBoss  = 85 + (ronde-1)*5;

        vector<pair<string,int>> sesiList = {
            {"Small Blind",goalSmall},
            {"High Blind",goalHigh},
            {"Boss Blind",goalBoss}
        };

        for (auto &sesi : sesiList) {

            if(exitToMenu) break;

            cout << "\n--- " << sesi.first << " ---\n";
            cout << "Target Skor: " << sesi.second << "\n";

            initializeDeck();

            int skorTerkumpul = 0;
            int kesempatanMain = 4;
            int kesempatanDiscard = 3;

            while (skorTerkumpul < sesi.second) {

                if (kesempatanMain <= 0) {
                    cout << "\nKesempatan main habis!\n";
                    gameOver = true;
                    break;
                }

                if (deck.empty()) {
                    cout << "\nDeck habis!\n";
                    gameOver = true;
                    break;
                }

                cout << "\n[Skor: " << skorTerkumpul << "/" << sesi.second << "]";
                cout << " | Main: " << kesempatanMain;
                cout << " | Discard: " << kesempatanDiscard;
                cout << " | $: " << currency;
                cout << " | Deck: " << deck.size() << "\n";

                int drawCount = min(8,(int)deck.size());
                vector<Card> hand;

                for(int i=0;i<drawCount;i++){
                    hand.push_back(deck.back());
                    deck.pop_back();
                }

                cout << "\nKartu di tangan:\n";
                for(size_t i=0;i<hand.size();i++){
                    cout << "(" << i+1 << ") ";
                    hand[i].tampilkan();
                    cout << "\n";
                }

                cout << "\nPilih kartu (maks 5): ";
                string input;
                getline(cin,input);

                stringstream ss(input);
                vector<int> indeksTerpilih;
                int pilihan;

                while(ss>>pilihan){
                    if(pilihan>=1 && pilihan<=hand.size()){
                        if(find(indeksTerpilih.begin(),indeksTerpilih.end(),pilihan-1)==indeksTerpilih.end()){
                            indeksTerpilih.push_back(pilihan-1);
                        }
                    }
                }

                if(indeksTerpilih.empty()){
                    cout<<"Tidak ada kartu valid.\n";
                    for(auto &c:hand) deck.push_back(c);
                    continue;
                }

                if(indeksTerpilih.size()>5)
                    indeksTerpilih.resize(5);

                vector<Card> chosenCards;
                for(int idx:indeksTerpilih)
                    chosenCards.push_back(hand[idx]);

                cout << "\n1. Play Hand\n";
                cout << "2. Discard\n";
                cout << "3. Exit To Menu\n";
                cout << "Pilih: ";

                string action;
                getline(cin,action);

                // ================= PLAY =================
                if(action=="1"){

                    PokerHandResult hasil = scorer.calculateScore(chosenCards);

                    IModifier* modA = ModifierFactory::createModifier("double");
                    IModifier* modB = ModifierFactory::createModifier("flat");

                    int skorFinal = hasil.skorTotal;

                    if(modA) skorFinal = modA->apply(skorFinal);
                    if(modB) skorFinal = modB->apply(skorFinal);

                    cout << "\n=== HASIL ===\n";
                    cout << "Kombinasi: "
                         << hasil.namaKombinasi
                         << " (" << hasil.skorKombinasi << ")\n";
                    cout << "Total Skor Turn: "
                         << skorFinal << "\n";

                    skorTerkumpul += skorFinal;
                    kesempatanMain--;

                    delete modA;
                    delete modB;
                }

                // ================= DISCARD =================
                else if(action=="2"){

                    if(kesempatanDiscard<=0){
                        cout<<"Kesempatan discard habis!\n";
                        continue;
                    }

                    cout<<"Yakin discard? (y/n): ";
                    string confirm;
                    getline(cin,confirm);

                    if(confirm=="y" || confirm=="Y"){
                        cout<<"Kartu dibuang.\n";
                        kesempatanDiscard--;
                    }
                    else{
                        cout<<"Discard dibatalkan.\n";
                        for(auto &c:hand) deck.push_back(c);
                        continue;
                    }
                }

                // ================= EXIT =================
                else if(action=="3"){
                    exitToMenu = true;
                    break;
                }

                cout<<"------------------------\n";
            }

            if(gameOver || exitToMenu)
                break;

            int bonus =
                kesempatanMain +
                kesempatanDiscard +
                (currency/5);

            currency += bonus;

            cout<<"\nSesi selesai!\n";
            cout<<"Bonus Currency: +"<<bonus<<"\n";
            cout<<"Total Currency: $"<<currency<<"\n";
        }
    }

    if(exitToMenu)
        cout<<"\nKembali ke Main Menu...\n";
    else if(!gameOver)
        cout<<"\n===== SELAMAT! ANDA MENANG =====\n";
    else
        cout<<"\n===== GAME OVER =====\n";

    cout<<"Tekan Enter untuk kembali...";
    string dummy;
    getline(cin,dummy);
}