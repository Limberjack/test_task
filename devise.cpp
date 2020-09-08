#include "devise.h"

Devise::Devise()
{
    for (int i = 0; i < 5; i++){
        try{
        printf("%i ", i);
        if (port.begin(i) >= 0)
            break;
        }
        catch(std::runtime_error a){
            if(i == 4)
                throw std::runtime_error("No serial devise is avaliable\n");
        }
    }
}

void Devise::begin(){
    std::thread sender([&](){
        port.send("AT+CIMI\r\n");
        port.reseave(this->IMSI, IMSI_SIZE);
        while(true){
            std::unique_lock<std::mutex> lk(mut);
            sleep(300);
            port.send("AT+QENG=\"servingcell\"\r\n");
            cond.notify_one();
        }
    });
    std::thread parser([&](){
        while(true){
            std::unique_lock<std::mutex> lk(mut);
            cond.wait(lk);
            port.reseave(this->AT_QENG, 512);
            this->display_info();
        }
    });
    sender.detach();
    parser.detach();
}


void Devise::display_info(){
    static std::vector<std::string> WCDMA_params ={"", "state : ", "", "MCC : ", "MNC : ",
                                                   "lac : ","cellid : ","uarfcn : ","PSC : ","rac : ","rscp : ",
                                                   "ecio : ","phych : ","sf : ","slot : ", "speech_code : ","comMod : "};
    static std::vector<std::string>  LTE_params = {"","state : ","","is_tdd : ","mcc : ","mnc : ",
                                                   "cellid : ","pcid : ","earfcn : ","freq_band_ind : ","ul_bandwidth : ",
                                                   "dl_bandwidth : ","tac : ","rsrp : ","rsrq : ","rssi : ","sinr : ","srxlev : "};
    std::vector<std::array<char, 20>> table;

    int message_pointer = 0;

    while(this->AT_QENG[message_pointer] != '\0'){
        table.push_back(std::array<char, 20>());
        for(int i = 0; i < 20; i++){
            if(AT_QENG[message_pointer] == ','){
                table.back()[i] = '\0';
                message_pointer++;
                break;
            }
            if(AT_QENG[message_pointer] == '\0'){
                table.back()[i] = '\0';
                break;
            }
            table.back()[i] = AT_QENG[message_pointer];
            message_pointer++;
        }
    }


    printf("\x1B[2J"); //clear console
    printf( "\x1B[0;0H");
    auto j = table.begin();
    if(strcmp(table.at(3).cbegin(), "LTE")){
        int counter = 0;
        for(auto i = LTE_params.begin(); i != LTE_params.end(); i++){
            if(counter == 10 || counter == 11){ //ul_badwidth and dl_badwidth
                switch(j->at(0)){
                case '0':
                    printf("%s1.4MHz\n", i->c_str());
                        continue;
                case '1':
                    printf("%s3MHz\n", i->c_str());
                    continue;
                case '2':
                    printf("%s5MHz\n", i->c_str());
                    continue;
                case '3':
                    printf("%s10MHz\n", i->c_str());
                    continue;
                case '4':
                    printf("%s15MHz\n", i->c_str());
                    continue;
                case '5':
                    printf("%s20MHz\n", i->c_str());
                    continue;
                }
            }
            else if(counter == 16){
                int dB = -20 + atoi(j->cbegin())*(50)/250;
                printf("%s%idB\n", i->c_str(), dB);
                continue;
            }
            printf("%s%s\n", i->c_str(), j->cbegin());
            j++;
        }
    }
    else{
        for(auto i = LTE_params.begin(); i != LTE_params.end(); i++){
            printf("%s%s\n", i->c_str(), j->cbegin());
            j++;
        }
    }


};
