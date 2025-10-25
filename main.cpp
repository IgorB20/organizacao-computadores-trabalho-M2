    //
    //  main.cpp
    //  trabalho-m1-organiz. de computadores
    //
    //  Igor Benedet e Kaio Gabriel Souza Rozini Hermann
    //  Trabalho 2 - Organização de Computadores



    #include <iostream>
    #include <fstream>
    #include <string>
    #include <bitset>
    #include <unordered_map>
    #include <algorithm>
    #include <cctype>
    #include <vector>

    using namespace std;


struct Instruction {
    string binary;
    string opcode;
    char type;

    unsigned rd = UINT_MAX;
    unsigned funct3 = UINT_MAX;
    unsigned rs1 = UINT_MAX;
    unsigned rs2 = UINT_MAX;
    unsigned funct7 = UINT_MAX;
    unsigned imm = UINT_MAX;

    void print() {
        cout << binary << " ";
        cout << "formato: " << type << " ";
        if(type == 'N') cout << "(NOP) ";
        cout << "opcode: " << opcode << " ";

        if (rd != UINT_MAX)     cout << "rd: " << rd << " ";
        if (funct3 != UINT_MAX) cout << "funct3: " << funct3 << " ";
        if (rs1 != UINT_MAX)    cout << "rs1: " << rs1 << " ";
        if (rs2 != UINT_MAX)    cout << "rs2: " << rs2 << " ";
        if (funct7 != UINT_MAX) cout << "funct7: " << funct7 << " ";
        if (imm != UINT_MAX)    cout << "imm: " << imm << " ";

        cout << endl;
    }
    
    void write(const string& filename) {
         ofstream file(filename, ios::app);
         if (!file.is_open()) {
             cerr << "Erro ao abrir o arquivo: " << filename << endl;
             return;
         }

         file << binary << " ";
         if(type == 'N') file << "(NOP) ";
         file << "formato: " << type << " ";
         file << "opcode: " << opcode << " ";

         if (rd != UINT_MAX)     file << "rd: " << rd << " ";
         if (funct3 != UINT_MAX) file << "funct3: " << funct3 << " ";
         if (rs1 != UINT_MAX)    file << "rs1: " << rs1 << " ";
         if (rs2 != UINT_MAX)    file << "rs2: " << rs2 << " ";
         if (funct7 != UINT_MAX) file << "funct7: " << funct7 << " ";
         if (imm != UINT_MAX)    file << "imm: " << imm << " ";

         file << endl;
         file.close();
     }
};

Instruction* createNop(){
    Instruction *nop = new Instruction();
    nop->binary = "00000000000000000000000000010011";
    nop->opcode = "0010011";
    nop->type = 'N';
    nop->rd = 0;
    nop->rs1 = 0;
    nop->rs2 = 0;
    return nop;
}


    vector<Instruction*> instructions;

    unordered_map<char, string> hexToBinMap = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"},
        {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"},
        {'8', "1000"}, {'9', "1001"}, {'A', "1010"}, {'B', "1011"},
        {'C', "1100"}, {'D', "1101"}, {'E', "1110"}, {'F', "1111"},
        {'a', "1010"}, {'b', "1011"}, {'c', "1100"}, {'d', "1101"},
        {'e', "1110"}, {'f', "1111"}
    };

    string hexToBinary(string hex) {
        string binary = "";

        for (char c : hex) {
            if (hexToBinMap.find(c) != hexToBinMap.end()) {
                binary += hexToBinMap[c];
            }
        }

        return binary;
    }

    string readOpcode(string binary){
        return binary.substr(25, 7); // 6:0
    }

    string readRd(string binary){
        return binary.substr(20, 5); // 7:11
    }

    string readFunct3(string binary) {
        return binary.substr(17, 3); // 12:14
    }

    string readRs1(string binary) {
        return binary.substr(12, 5); // 15:19
    }

    string readRs2(string binary) {
        return binary.substr(7, 5); // 20:24
    }

    string readFunct7(string binary) {
        return binary.substr(0, 7); // 25:31
    }

    void readB(string instruction){
        /*
            opcode 0:6
            rd 7:11
            rs1
            rs2
            funct3
            imm 11
            imm 4:1
            imm 10:5
            imm 12
        */
        char type = 'B';
        string opcode = readOpcode(instruction);
        string rs1Bin = readRs1(instruction);
        string rs2Bin = readRs2(instruction);
        string funct3Bin = readFunct3(instruction);
        string imm4_1    = instruction.substr(31-11, 4);
        string imm11    = instruction.substr(31-7, 1);
        string imm10_5  = instruction.substr(31-30, 6);
        string imm12 = instruction.substr(0, 1);
        
        //adiciono um 0 para manter em 14 bits ( offset de branch no RISC-V é múltiplo de 2 )
        string immTotal = imm12 + imm11 + imm10_5 + imm4_1 + "0";
        
        std::bitset<5> rs1(rs1Bin);
        std::bitset<5> rs2(rs2Bin);
        std::bitset<3> funct3(funct3Bin);
        std::bitset<13> immBits(immTotal);
        
        // sign-extension
        int immValue;
        if (immBits.test(12)) {// se bit de sinal = 1 negativo
            immValue = static_cast<int>(immBits.to_ulong() | 0xFFFFE000);
        } else {
            immValue = static_cast<int>(immBits.to_ulong());
        }
         
        Instruction *b = new Instruction();
        b->type = type;
        b->binary = instruction;
        b->opcode = opcode;
        b->rs1 = rs1.to_ulong();
        b->rs2 = rs2.to_ulong();
        b->funct3 = funct3.to_ulong();
        b->imm = immValue;
        
        instructions.push_back(b);

    }

    void readU(string instruction){
        
        /*
            opcode 0:6
            rd 7:11
            imm 12:31
        */
        char type = 'U';

        string opcode = readOpcode(instruction);
        string rdBin = readRd(instruction);
        string imm = instruction.substr(0, 20);
        std::bitset<20> immBits(imm);
            
        std::bitset<5> rd(rdBin);
        

        Instruction *u = new Instruction();
        u->type = type;
        u->binary = instruction;
        u->opcode = opcode;
        u->rd = rd.to_ulong();
        u->imm = immBits.to_ulong();
        
        instructions.push_back(u);
    }

    void readJ(string instruction){
        /*
         opcode 0:6
         rd 7:11
         imm[19:12]
         imm[11]
         imm[10:1]
         imm[20]
         */
        
        char type = 'J';
        string opcode = readOpcode(instruction);
        string rdBin = readRd(instruction);
        string imm20    = instruction.substr(0, 1);
        string imm10_1  = instruction.substr(1, 10);
        string imm11    = instruction.substr(11, 1);
        string imm19_12 = instruction.substr(12, 8);
        string immTotal = imm20 + imm19_12 + imm11 + imm10_1 + "0";
        std::bitset<21> immBits(immTotal);
        std::bitset<5> rd(rdBin);
        
        // sign-extension
        int immDec;
        if (immBits.test(20)) {// se bit de sinal = 1 negativo
            immDec = static_cast<int>(immBits.to_ulong() | 0xFFE00000);
        } else {
            immDec = static_cast<int>(immBits.to_ulong());
        }
        
        
        Instruction *j = new Instruction();
        j->type = type;
        j->binary = instruction;
        j->opcode = opcode;
        j->rd = rd.to_ulong();
        j->imm = immDec;
        
        instructions.push_back(j);
        
    }

    void readR(string instruction) {
        /*
            opcode 0:6
            rd 7:11
            funct3[12:14]
            rs1[15: 19]
            rs2[20:24]
            funct7[25]
        */
        char type = 'R';
        string opcode;
        string rdBin, funct3Bin, rs1Bin, rs2Bin, funct7Bin;


        opcode = readOpcode(instruction);
        rdBin = readRd(instruction);
        funct3Bin = readFunct3(instruction);
        rs1Bin = readRs1(instruction);
        rs2Bin = readRs2(instruction);
        funct7Bin = readFunct7(instruction);

        std::bitset<5> rd(rdBin);
        std::bitset<3> funct3(funct3Bin);
        std::bitset<5> rs1(rs1Bin);
        std::bitset<5> rs2(rs2Bin);
        std::bitset<5> funct7(funct7Bin);
        
        Instruction *r = new Instruction();
        r->type = type;
        r->binary = instruction;
        r->opcode = opcode;
        r->rd = rd.to_ulong();
        r->funct3 = funct3.to_ulong();
        r->rs1 = rs1.to_ulong();
        r->rs2 = rs2.to_ulong();
        r->funct7 = funct7.to_ulong();
        
        instructions.push_back(r);
    }

    void readI(string instruction) {
        /*
            opcode 0:6
            rd 7:11
            funct3[12:14]
            rs1[15: 19]
            imm[20:31]
        */
        char type = 'I';
        string opcode;
        string rdBin, funct3Bin, rs1Bin, imm;

        opcode = readOpcode(instruction);
        rdBin = readRd(instruction);
        funct3Bin = readFunct3(instruction);
        rs1Bin = readRs1(instruction);
        imm = instruction.substr(0, 12);
        
        std::bitset<5> rd(rdBin);
        std::bitset<3> funct3(funct3Bin);
        std::bitset<5> rs1(rs1Bin);
        std::bitset<12> immBits(imm);

        // sign-extension
        int immDec;
        if (immBits.test(11)) {// se bit de sinal = 1 (negativo)
            immDec = static_cast<int>(immBits.to_ulong() | 0xFFFFF000);
        } else {
            immDec = static_cast<int>(immBits.to_ulong());
        }
        
        Instruction *i = new Instruction();
        i->type = type;
        i->binary = instruction;
        i->opcode = opcode;
        i->rd = rd.to_ulong();
        i->funct3 = funct3.to_ulong();
        i->rs1 = rs1.to_ulong();
        i->imm = immDec;
        
        instructions.push_back(i);
    }

    void readS(string instruction) {
        
        /*
           
        */

        
        char type = 'S';
        string opcode;
        string imm4_0, funct3Bin, rs1Bin, rs2Bin, imm11_5;

        opcode = readOpcode(instruction);
        funct3Bin = readFunct3(instruction);
        rs1Bin = readRs1(instruction);
        rs2Bin = readRs2(instruction);
        imm4_0 = readRd(instruction);
        imm11_5 = readFunct7(instruction);
        string immTotal = imm11_5 + imm4_0;
        
        std::bitset<12> immBits(immTotal);

        // sign-extension
        int immDec;
        if (immBits.test(11)) {// se bit de sinal = 1 negativo
            immDec = static_cast<int>(immBits.to_ulong() | 0xFFFFF000);
        } else {
            immDec = static_cast<int>(immBits.to_ulong());
        }
        
        std::bitset<3> funct3(funct3Bin);
        std::bitset<5> rs1(rs1Bin);
        std::bitset<5> rs2(rs2Bin);
        
        Instruction *s = new Instruction();
        s->type = type;
        s->binary = instruction;
        s->opcode = opcode;
        s->rs1 = rs1.to_ulong();
        s->rs2 = rs2.to_ulong();
        s->funct3 = funct3.to_ulong();
        s->imm = immDec;
        
        instructions.push_back(s);
        
    }


    bool isHexadecimal(string input){
        return input.size() == 8;
    }


    int main(int argc, const char * argv[]) {
            
        vector<string> binaries;
        
        string line;
        ifstream MyReadFile("fib_rec_hexadecimal.txt");
        while (getline(MyReadFile, line)) {
                line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
                if (!line.empty()) {
                    string binary = line;
                    if(isHexadecimal(line)){
                        binary = hexToBinary(line);
                    }
                    
                    binaries.push_back(binary);
                }
        }
        
        for(string bin : binaries){
            string opcode = readOpcode(bin);
           
             if(opcode == "0110111") readU(bin);
             if(opcode == "1101111") readJ(bin);
             if(opcode == "1100111"
                || opcode == "0010011"
                || opcode == "0001111"
                || opcode == "0000011") readI(bin);
             if(opcode == "1100011") readB(bin);
             if(opcode == "0100011") readS(bin);
             if(opcode == "0110011") readR(bin);
        }
        
        // Hazard de dados
        // sem forwarding
        int index = 0;
        for(Instruction *instruction : instructions){
            instruction->print();
            instruction->write("3-A.txt");
            
            if((instruction->type == 'I'
               || instruction->type == 'R'))
            {
        
                if(index + 1 < instructions.size()){ // uma instrucao a frente
                    Instruction *nextInstruction = instructions[index+1];
                    if(instruction->rd == nextInstruction->rs1 || instruction->rd == nextInstruction->rs2){
                        Instruction *nop = createNop();
                        nop->write("3-A.txt");
                        nop->write("3-A.txt");
                        cout << "Hazard de dados encontrado a partir da instruçao " << index + 1 << " inserir 2 NOPS aqui" <<endl;
                    }else  if(index + 2 < instructions.size()){ // duas instrucoes a frente
                        Instruction *nextInstruction = instructions[index+2];
                        if(instruction->rd == nextInstruction->rs1 || instruction->rd == nextInstruction->rs2){
                            Instruction *nop = createNop();
                            nop->write("3-A.txt"); // na verdade esse nop precisa ser inserido DEPOIS da proxima instrucao
                            cout << "Hazard de dados encontrado a partir da instruçao " << index + 1 << " inserir 1 NOPS aqui" <<endl;
                        }
                    }
                }
            }
            index++;
        }
        
        cout << endl;
        
        // Hazard de dados
        // com forwarding
        index = 0;
        for(Instruction *instruction : instructions){
            instruction->print();
            instruction->write("3-B.txt");
            
            if((instruction->opcode == "0000011")) //considera apenas LW
            {
        
                if(index + 1 < instructions.size()){ // uma instrucao a frente
                    Instruction *nextInstruction = instructions[index+1];
                    if(instruction->rd == nextInstruction->rs1 || instruction->rd == nextInstruction->rs2){
                        Instruction *nop = createNop();
                        nop->write("3-B.txt");
                        cout << "Hazard de dados encontrado a partir da instruçao " << index + 1 << " inserir 1 NOPS aqui" <<endl;
                    }
                }
            }
            
            index++;
        }
        
        cout << endl;
        index = 0;
        // Hazard de controle com e sem forwarding
        for(Instruction *instruction : instructions){
            instruction->print();
            instruction->write("4-A-B.txt");
            if(instruction->type == 'B'|| instruction->type == 'J' || (instruction->type == 'I' && instruction->opcode == "1100111")){ // considera opcode da jalr
                Instruction *nop = createNop();
                nop->write("4-A-B.txt");
                cout << "Hazard de controle encontrado a partir da instruçao " << index + 1 << " inserir 1 NOP aqui" <<endl;
            }
            index++;
        }
        
        for(Instruction *instruction : instructions){
            delete instruction;
        }
            
    }
