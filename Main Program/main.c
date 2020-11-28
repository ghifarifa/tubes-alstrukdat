
#include "../ADT/Mesin Karakter & Kata/mesin_kar.c"
#include "../ADT/Mesin Karakter & Kata/mesinkata.c"
#include "../ADT/Graph/graph.c"
// #include "../ADT/Array Dinamis/list.c"
#include "../ADT/Stack/stackt.c"

#include "string.h"
#include "user-interface.c"
#include "stdlib.h"

int main(){
    /* ---- TAMPILAN MENU UTAMA BANGET ---- */
    system("cls");
    static FILE * fsave;
    fsave = fopen("save.txt","a");
    char InputCommand[100];
    MainMenu();
    scanf("%s",InputCommand);
    if (strcmp(InputCommand,"START")==0){
        /* INISIALISASI STRUKTUR DATA PENYIMPANAN DATA PENTING */
        MATRIKS MapMatrix; // -> Nyimpen data Peta, letak pemain, letak pelanggan
        Graph G; // -> Nyimpen konektivitas titik2 pada peta
        MATRIKS GraphMatrix; // -> Nyimpen Adjacency Matriks untuk titik2 pada peta
        int UangPemain=4500; // -> Default uang pemain
        Stack Rakitan; // -> Nyimpen Rakitan yang lagi dibangun
        Stack CurrentPesanan; // -> Nyimpen stack komponen2 pesanan sekarang
        boolean lagiBuild=false; // -> Kalo udah startbuild-> true, klo ga false
        List InventoryPemain; // -> Nyimpen inventory pemain

        /* Kosongkan semua yang bakal dinamis */
        CreateStackEmpty(&Rakitan);
        
        
        /* PEMBACAAN FILE KONFIGURASI */

        STARTKATA("../ADT/Matriks/map.txt");
        //2 karena tambah ruang untuk border
        int BrsPita = 2;
        int KolPita = 2;
        //BAG 1. AKUISISI - Misal akuisisi 10 & 15 dari pita
        for (int a=0;a<CKata.Length;a++){
            char temp = CKata.TabKata[a]; // berisi nilai akuisisi
            int convert = temp - '0'; // mengubah dari char ke int
            BrsPita += pow(10,CKata.Length-a-1)*convert;
        }
        ADVKATA();
        for (int b = 0;b<CKata.Length;b++){
            char temp = CKata.TabKata[b]; // berisi nilai akuisisi
            int convert = temp - '0'; // mengubah dari char ke int
            KolPita += pow(10,CKata.Length-b-1)*convert;
        }
        MakeMATRIKS(BrsPita,KolPita,&MapMatrix);
        EmptyMatrix(&MapMatrix);
        
        // sampe sini baca pita AMAN

        //BAG 2. TOTAL TITIK - Misal total titik di peta ada 9
        ADVKATA();
        int TitikTotal=0;
        for (int c=1;c<CKata.Length;c++){ /* mulai dari c=1 karena Lengthnya tambah akibat ada blank*/
            char temp = CKata.TabKata[c]; // berisi nilai akuisisi
            int convert = temp - '0'; // mengubah dari char ke int
            TitikTotal += pow(10,CKata.Length-c-1)*convert;
        }
        TitikTotal-=2; //karena kita mulai dari -1
        // baca titik total AMAN

        //BAG 3. PEMETAAN - Start pasang-pasang titik ke peta,
        for(int I=-1;I<=TitikTotal;I++) {
            POINT CTemp;
            ADVKATA();
            char Type = CKata.TabKata[1]; /* Bukan 0 karena indeks 0 isinya blank*/
            ADVKATA();
            int Absis=0;
            for (int d=0;d<CKata.Length;d++){
                char temp = CKata.TabKata[d]; // berisi nilai akuisisi
                int convert = temp - '0'; // mengubah dari char ke int
                Absis += pow(10,CKata.Length-d-1)*convert;
            }
            ADVKATA();
            int Ordinat=0;
            for (int e=0;e<CKata.Length;e++){
                char temp = CKata.TabKata[e]; // berisi nilai akuisisi
                int convert = temp - '0'; // mengubah dari char ke int
                Ordinat += pow(10,CKata.Length-e-1)*convert;
            }
            SimpanPoint(&CTemp,Absis,Ordinat,Type,I);
            if (I==-1){
                CurrentAbsis(MapMatrix)=Absis;
                CurrentOrdinat(MapMatrix)=Ordinat;
            }
            IsiPoint(&MapMatrix,CTemp);
        }
        ADVKATA(); 
        MakeZeroMatrix(&GraphMatrix,TitikTotal,TitikTotal);               

        for (int i=BrsMin;i<NBrsEff(GraphMatrix);i++){
            for (int j=KolMin;j<NKolEff(GraphMatrix);j++){
                if (CKata.Length == 1){
                    char temp = CKata.TabKata[0]; 
                    int konversi = temp - '0';
                    ElmtMatrix(GraphMatrix,i,j)=konversi;
                    ADVKATA();
                }
                else { // CKata.Length = 2 jika membaca baris baru (enter)
                    char temp = CKata.TabKata[1]; 
                    int konversi = temp - '0';
                    ElmtMatrix(GraphMatrix,i,j)=konversi;
                    ADVKATA();
                }
            }
        }
        initGraph(&G,TitikTotal);
        for (int i=BrsMin;i<NBrsEff(GraphMatrix);i++){
            for (int j=KolMin;j<NKolEff(GraphMatrix);j++){
                if (ElmtMatrix(GraphMatrix,i,j)==1){
                    AddLink(&G,i-1,j-1);
                }
            }
        }
        int CurrentDeliveryLoc = 3;
        POINT PointDeliveryLoc = SearchMatrix(MapMatrix,CurrentDeliveryLoc);
        White;


        
        /* ---- TAMPILAN MENU SETELAH IN-GAME ---- */
        system("cls");
        printf("Welcome to Santo's World...\n");
        ShowUI();
        Cyan; //set color Cyan
        scanf("%s",InputCommand);
        while(strcmp(InputCommand,"EXIT")!=0){
            White; //set color White
            system("cls");
            
            /* ---- DAFTAR COMMAND UTAMA PERMAINAN ---- */
            /* COMMAND 1: MOVE */
            if (strcmp(InputCommand,"MOVE")==0){            
                ShowValidTargets(G,CurrentPos(MapMatrix));
                int InputTarget;
                scanf("%d",&InputTarget);
                addressGraph P1;
                P1=FirstGraph(G);
                while (InfoGraph(P1)!=CurrentPos(MapMatrix)){
                    P1=NextGraph(P1);
                }
                address AdrTarget = First(Link(P1));                
                if(InputTarget>NbElmt(Link(P1)) || InputTarget<1){
                    printf("That area is not accessible, please impute according to shown indices (1-%d).\n", NbElmt(Link(P1)));
                }
                else{
                    fputs("MOVE ",fsave); // Menyimpan command ke save file
                    // Save input perpindahan lokasi ke save file
                    char input[3];
                    sprintf(input,"%d ",InputTarget); // Mengubah int menjadi char
                    // "%d " membuat ada spasi di input
                    const char* p = input; // mengubah char ke const char* supaya menjadi parameter fputs
                    fputs(p,fsave);
                    // Selesai disave
                    for (int i=1;i<InputTarget;i++){
                        AdrTarget=NextGraph(AdrTarget);
                    }
                    infotypeGraph IndexTarget;
                    IndexTarget=InfoGraph(AdrTarget);
                    Move(G,CurrentPos(MapMatrix),IndexTarget,&MapMatrix);
                    printf("You have reached ");
                    switch(CurrentPos(MapMatrix)){
                        case -1:
                            printf("your\033[0;33m Base");break;
                        case 0:
                            printf("the\033[0;32m Shop");break;
                        default:
                            printf("Pelanggan\033[0;31m %d",CurrentPos(MapMatrix));break;
                    }
                    White;
                    printf(".\n");                   
                }
            }
            /* COMMAND 2 : STATUS */
            else if (strcmp(InputCommand,"STATUS")==0){  
                int Build = 3; //Ambil dari Queue
                int Order = 1; //nanti variablenya disesuain sm yg lain
                int length = 2; //Contoh contoh contoh
                
                List Inventory = MakeList(); //BUAT DUMMY DOANG
                for (int i=0;i<length;i++){
                    ElTypeList ElInventory = InputLElType();
                    InsertLAt(&Inventory,ElInventory,i);
                }

                printf("Uang tersisa: $%d\n",UangPemain);
                printf("Build yang sedang dikerjakan: pesanan %d untuk pelanggan %d.\n", Order, Build);
                printf("Lokasi: pemain sedang berada pada ");
                switch(CurrentPos(MapMatrix)){
                    case -1:
                    printf("Base");break;
                    
                    case 0:
                    printf("Shop");break;

                    default:
                    printf("Pelanggan %d",CurrentPos(MapMatrix));
                }
                printf("\n");
                printf("Inventory anda:\n");
                int j = 1;
                for (int i=0;i<LengthList(Inventory);i++){
                    if (Jumlah(ListElmt(Inventory,i)) > 0){
                        printf("%d. %s (%d)\n",j,Nama(ListElmt(Inventory,i)),Jumlah(ListElmt(Inventory,i)));
                        j++;
                    }
                }            
            }


            /* COMMAND 3 : CHECKORDER */
            else if (strcmp(InputCommand,"CHECKORDER")==0){

            }

            /* COMMAND 4 : STARTBUILD */
            else if (strcmp(InputCommand,"STARTBUILD")==0){
                STARTBUILD(&Rakitan,&lagiBuild,1,2);
            }

            /* COMMAND 5 : FINISHBUILD */
            else if (strcmp(InputCommand,"FINISHBUILD")==0){
                FINISHBUILD(CurrentPesanan,Rakitan,&lagiBuild,1,2);
            }

            /* COMMAND 6 : ADDCOMPONENT */
            else if(strcmp(InputCommand,"ADDCOMPONENT")==0){
                ADDCOMPONENT(&Rakitan, &InventoryPemain);
            }
            /* COMMAND 7 : REMOVECOMPONENT */
            else if (strcmp(InputCommand,"REMOVECOMPONENT")==0){
                REMOVECOMPONENT(&Rakitan,&InventoryPemain);
            }
            /* COMMAND 9: DELIVER */
            else if (strcmp(InputCommand,"DELIVER")==0){
                if(CurrentAbsis(MapMatrix)==Absis(PointDeliveryLoc) && CurrentOrdinat(MapMatrix)==Ordinat(PointDeliveryLoc)){
                    /* fputs("DELIVER ",fsave); */
                    printf("Item successfully delivered to Pelanggan %d!\n",CurrentPos(MapMatrix));
                    //harusnya ada Dequeue disini
                }
                else{
                    printf("This is not the right address for your delivery!\n");
                }
            }

            /* COMMAND 11: SAVE */
            else if (strcmp(InputCommand,"SAVE")==0){
                fputs(". ",fsave); // akhir dari program yang disave ditandai mark
                printf("Lokasi save file:");
                printf("C/User/Documents/GitHub/tubes-alstrukdat-kel2/ADT/Mesin Karakter & Kata/save.txt\n");
                printf("Game berhasil di save!\n");
            }
            /* COMMAND 12: MAP */
            else if (strcmp(InputCommand,"MAP")==0){   
                PrintMap(MapMatrix); //AMAN
            
            }
            /* WRONG INPUT */
            else{
                printf("Input Anda salah!\n");
            }
            ShowUI();
            Cyan;
            scanf("%s",InputCommand);       
        }
        
        White;
        fclose(fsave);
        printf("Thank you for playing!\n");
        


    }
    return 0;
}