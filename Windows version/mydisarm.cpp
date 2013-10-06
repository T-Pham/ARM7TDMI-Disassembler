#ifndef _mydisarm_
#define _mydisarm_

#include "stdafx.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
using namespace std;

const string __unDefIns = ";\tundefined instruction"; // instruction not supported


//----------------------------------------
void WordToBinary(unsigned long wordIns, bool binIns[32]) {
	for (int i = 0; i < 32; i++) {
		binIns[i] = wordIns % 2;
		wordIns /= 2;
	}
	return;
}

//----------------------------------------
int PowOf2(int exp) {
	if (exp < 0) return -1; // negative exponent not accepted

	int result = 1;
	for (int i = 1; i <= exp; i++) result *= 2;
	return result;
}

//----------------------------------------
int BinToUnsignedInt(bool Ins[32], int position, int length) {
	// Invalid input cases
	if ((position < 0) || (position > 31)) return -1;
	if ((length < 0) || (length > 32)) return -1;
	if (position + 1 - length < 0) return -1;
	if (length == 0) return 0; // maybe valid

	int result = 0;
	int exp = length;
	for (int i = position; i >= position + 1 - length; i--) {
		exp--;
		result += Ins[i] * PowOf2(exp);
	}
	return result;
}

//----------------------------------------
string IntToString(int anInteger) {
	stringstream strstream;
	string result = "";
	strstream << anInteger;
	result = strstream.str();
	return result;
}

//----------------------------------------
string IntToHexString(unsigned int anInteger) {
	stringstream strstream;
	string result = "0x";
	strstream.fill('0'); 							 /* set */
	strstream.setf(ios::hex | ios::uppercase);		/*-----*/
	strstream << setw(8) << hex << anInteger;
	strstream.fill(' ');							 /*-------*/
	strstream.unsetf(ios::hex | ios::uppercase);	/* unset */
	result += strstream.str();
	return result;
}

//----------------------------------------
string getCondition(bool Ins[32]) {
	string hash = "";
	for (int i = 31; i >= 28; i--) {
		if (Ins[i]) hash += "1";
		else hash += "0";
	}
	if (hash == "0000") return "EQ";
	if (hash == "0001") return "NE";
	if (hash == "0010") return "HS"; // || CS
	if (hash == "0011") return "LO"; // || CC
	if (hash == "0100") return "MI";
	if (hash == "0101") return "PL";
	if (hash == "0110") return "VS";
	if (hash == "0111") return "VC";
	if (hash == "1000") return "HI";
	if (hash == "1001") return "LS";
	if (hash == "1010") return "GE";
	if (hash == "1011") return "LT";
	if (hash == "1100") return "GT";
	if (hash == "1101") return "LE";
	if (hash == "1110") return "";   // AL
	if (hash == "1111") return "NV";
	return ""; // this line never runs
}

//----------------------------------------
string getRegister(bool Ins[32], int position) {
	if (position < 3) return "";
	int hash = BinToUnsignedInt(Ins, position, 4);
	string result = "";

	switch (hash) {
		case  0: result = "R0";  break;
		case  1: result = "R1";  break;
		case  2: result = "R2";  break;
		case  3: result = "R3";  break;
		case  4: result = "R4";  break;
		case  5: result = "R5";  break;
		case  6: result = "R6";  break;
		case  7: result = "R7";  break;
		case  8: result = "R8";  break;
		case  9: result = "R9";  break;
		case 10: result = "R10"; break;
		case 11: result = "R11"; break;
		case 12: result = "IP";  break; // Intra-Procedure-call scratch register
		case 13: result = "SP";  break; // Stack Pointer
		case 14: result = "LR";  break; // Link Register
		case 15: result = "PC";  break; // Program Counter
		default: result = "";    break; // this line never runs
	}
	return result;
}

//----------------------------------------
string getShifterOperand(bool Ins[32]) {
	// Shifter Operand case No 1
	if (Ins[25]) { // if (I == 1)
		int rotate_imm_n = 2 * BinToUnsignedInt(Ins, 11, 4);
		bool immValue[32];
		for (int i = 0; i < 32; i++) {
			if (i > 7) immValue[i] = 0;
			else immValue[i] = Ins[i];
		}
		bool bit0;
		for (int i = 0; i < rotate_imm_n; i++) { // do rotate bits
			bit0 = immValue[0];
			for (int k = 1; k <= 31; k++) immValue[k - 1] = immValue[k];
			immValue[31] = bit0;
		}
		unsigned int immValue32 = BinToUnsignedInt(immValue, 31, 32);
		string result = IntToHexString(immValue32);
		return result;
	}

	// (I == 0)
	string result = "";
	// Shifter Operand case No 2
	if (!(BinToUnsignedInt(Ins, 11, 8))) { // if (all bits from bit 11 to bit 4 is 0)
		result = getRegister(Ins, 3);
		return result;
	}
	
	// Shifter Operand case No 11
	if (BinToUnsignedInt(Ins, 11, 8) == 6) { // if (bit[11-4] is 00000110)
		result = getRegister(Ins, 3);
		result += ", RRX";
		return result;
	}
	
	int hash = BinToUnsignedInt(Ins, 6, 3);
	string shifttype = "";
	string Rm = getRegister(Ins, 3);
	string Rs = "";
	string shift_imm = "";
	int shift_imm_n;
	switch(hash) { //pass 1
		case 0: case 1: shifttype = "LSL"; break; // Shifter Operand case No 3, 4
		case 2: case 3: shifttype = "LSR"; break; // Shifter Operand case No 5, 6
		case 4: case 5: shifttype = "ASR"; break; // Shifter Operand case No 7, 8
		case 6: case 7: shifttype = "ROR"; break; // Shifter Operand case No 9, 10
		default: break; // this line never runs
	}
	switch(hash) { //pass 2
		case 0: case 2: case 4: case 6:
			shift_imm_n = BinToUnsignedInt(Ins, 11, 5);
			shift_imm = IntToString(shift_imm_n);
			result = Rm + ", " + shifttype + " #" + shift_imm;
			return result;
			break;
		case 1: case 3: case 5: case 7:
			if (Ins[7]) { // if (bit 7 == 1)
				result = __unDefIns;
				return result;
			}
			Rs = getRegister(Ins, 11);
			result = Rm + ", " + shifttype + " " + Rs;
			return result;
			break;
		default: break; // this line never runs
	}
	return result; // this line never runs
}

//----------------------------------------
string getAddressingMode2(bool Ins[32]) {
	// addressing_mode 2 case No 1, 4, 7
	if (!Ins[25]) { // if (I == 0)
		string result = "";
		string Rn = getRegister(Ins, 19);
		int offset_12_n = BinToUnsignedInt(Ins, 11, 12);
		string offset_12 = IntToString(offset_12_n);
		if (!Ins[23]) offset_12 = "-" + offset_12; // if (U == 0)
	
		// addressing_mode 2 case No 1
		if ((Ins[24]) && (!Ins[21])) { // if ((P == 1) && (W == 0))
			result = "[" + Rn + ", #" + offset_12 + "]";
			return result;
		}
	
		// addressing_mode 2 case No 4
		if ((Ins[24]) && (Ins[21])) { // if ((P == 1) && (W == 1))
			result = "[" + Rn + ", #" + offset_12 + "]!";
			return result;
		}
	
		// addressing_mode 2 case No 7
		if ((!Ins[24]) && (!Ins[21])) { // if ((P == 0) && (W == 0))
			result = "[" + Rn + "], #" + offset_12;
			return result;
		}
	
		//else
		return __unDefIns;
	}

	// (I == 1)
	int hash = BinToUnsignedInt(Ins, 11, 8);

	// addressing_mode 2 case No 2, 5, 8
	if (!hash) { // if (all bits from bit 11 to bit 4 is 0)
		string result = "";
		string Rn = getRegister(Ins, 19);
		string Rm = getRegister(Ins, 3);
		if (!Ins[23]) Rm = "-" + Rm; // if (U == 0)
	
		// addressing_mode 2 case No 2
		if ((Ins[24]) && (!Ins[21])) { // if ((P == 1) && (W == 0))
			result = "[" + Rn + ", " + Rm + "]";
			return result;
		}
	
		// addressing_mode 2 case No 5
		if ((Ins[24]) && (Ins[21])) { // if ((P == 1) && (W == 1))
			result = "[" + Rn + ", " + Rm + "]!";
			return result;
		}
	
		// addressing_mode 2 case No 8
		if ((!Ins[24]) && (!Ins[21])) { // if ((P == 0) && (W == 0))
			result = "[" + Rn + "], " + Rm;
			return result;
		}
	
		//else
		return __unDefIns;
	}

	if (Ins[4]) return __unDefIns; // if (bit 4 == 1)

	// addressing_mode 2 case No 3, 6, 9
	string result = "";
	string Rn = getRegister(Ins, 19);
	string Rm = getRegister(Ins, 3);
	if (!Ins[23]) Rm = "-" + Rm; // if (U == 0)
	string shift_imm = "";
	int shift_imm_n;
	shift_imm_n = BinToUnsignedInt(Ins, 11, 5);
	shift_imm = IntToString(shift_imm_n);
	string shifttype = "";
	int hash_shifttype = BinToUnsignedInt(Ins, 6, 2);
	switch (hash_shifttype) {
		case 0: shifttype = "LSL"; break;
		case 1: shifttype = "LSR"; break;
		case 2: shifttype = "ASR"; break;
		case 3:
			if (shift_imm_n == 0) shifttype = "RRX";
			else shifttype = "ROR";
			break;
		default: break; // this line never runs
	}
	string shift_field = shifttype;
	if (shifttype != "RRX") shift_field += (" #" + shift_imm);
	
	// addressing_mode 2 case No 3
	if ((Ins[24]) && (!Ins[21])) { // if ((P == 1) && (W == 0))
		result = "[" + Rn + ", " + Rm + ", " + shift_field + "]";
		return result;
	}

	// addressing_mode 2 case No 6
	if ((Ins[24]) && (Ins[21])) { // if ((P == 1) && (W == 1))
		result = "[" + Rn + ", " + Rm + ", " + shift_field + "]!";
		return result;
	}

	// addressing_mode 2 case No 9
	if ((!Ins[24]) && (!Ins[21])) { // if ((P == 0) && (W == 0))
		result = "[" + Rn + "], " + Rm + ", " + shift_field;
		return result;
	}

	//else
	return __unDefIns;
}

//----------------------------------------
string getAddressingMode3(bool Ins[32]) {
	if (!(Ins[7] && Ins[4])) return __unDefIns; // if ((bit 7 == 0) || (bit 4 == 0))

	// addressing_mode 3 case No 1, 3, 5
	if (Ins[22]) { // if (I == 1)
		string result = "";
		string Rn = getRegister(Ins, 19);
		bool immed_b[32];
		immed_b[0] = Ins[0]; // get immedL
		immed_b[1] = Ins[1];
		immed_b[2] = Ins[2];
		immed_b[3] = Ins[3];
		immed_b[4] = Ins[8]; // get immedH
		immed_b[5] = Ins[9];
		immed_b[6] = Ins[10];
		immed_b[7] = Ins[11];
		for (int i = 8; i < 32; i++) immed_b[i] = 0;
		int immed_n = BinToUnsignedInt(immed_b, 7, 8);
		string immed = IntToString(immed_n);
		if (!Ins[23]) immed = "-" + immed; // if (U == 0)
	
		// addressing_mode 3 case No 1
		if ((Ins[24]) && (!Ins[21])) { // if ((P == 1) && (W == 0))
			result = "[" + Rn + ", #" + immed + "]";
			return result;
		}
	
		// addressing_mode 3 case No 3
		if ((Ins[24]) && (Ins[21])) { // if ((P == 1) && (W == 1))
			result = "[" + Rn + ", #" + immed + "]!";
			return result;
		}
	
		// addressing_mode 3 case No 5
		if ((!Ins[24]) && (!Ins[21])) { // if ((P == 0) && (W == 0))
			result = "[" + Rn + "], #" + immed;
			return result;
		}
	
		//else
		return __unDefIns;
	}

	// (I == 0)
	int SBZ = BinToUnsignedInt(Ins, 11, 4);
	if (SBZ) return __unDefIns; // if (SBZ != 0)

	// addressing_mode 3 case No 2, 4, 6
	string result = "";
	string Rn = getRegister(Ins, 19);
	string Rm = getRegister(Ins, 3);
	if (!Ins[23]) Rm = "-" + Rm; // if (U == 0)

	// addressing_mode 3 case No 2
	if ((Ins[24]) && (!Ins[21])) { // if ((P == 1) && (W == 0))
		result = "[" + Rn + ", " + Rm + "]";
		return result;
	}

	// addressing_mode 3 case No 4
	if ((Ins[24]) && (Ins[21])) { // if ((P == 1) && (W == 1))
		result = "[" + Rn + ", " + Rm + "]!";
		return result;
	}

	// addressing_mode 3 case No 6
	if ((!Ins[24]) && (!Ins[21])) { // if ((P == 0) && (W == 0))
		result = "[" + Rn + "], " + Rm;
		return result;
	}

	//else
	return __unDefIns;
}

//----------------------------------------
string analyzeRegisterSet(int linear[16], int position) {
	if ((position < 0) || (position > 15)) return "";
	int linearI = linear[position];
	if (linearI < 1) return "";
	if ((position + linearI - 1 < 0) || (position + linearI - 1 > 15)) return "";

	string RegisterSet[16] = {"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7", "R8", "R9", "R10", "R11", "IP", "SP", "LR", "PC"};
	string result = RegisterSet[position];
	string tailReg = RegisterSet[position + linearI - 1];
	if (linearI == 1) return result;
	if (linearI == 2) {
		result += (", " + tailReg);
		return result;
	}
	//(linearI >= 3)
	result += ("-" + tailReg);
	return result;
}

//----------------------------------------
string getAddressingMode4(bool Ins[32]) {
	int hash = BinToUnsignedInt(Ins, 24, 2);
	string result = "";
	switch (hash) {
		case 0: result = "DA"; break;
		case 1: result = "IA"; break;
		case 2: result = "DB"; break;
		case 3: result = "IB"; break;
		default: break; // this line never runs
	}
	return result;
}

//----------------------------------------
string getExclamation(bool Ins[32]) {
	if (Ins[21]) return "!";
	else return ""; // NULL
}

//----------------------------------------
string getCircumflex(bool Ins[32]) {
	if (Ins[22]) return "^";
	else return ""; // NULL
}

//----------------------------------------
string getRegisterList(bool Ins[32]) {
	int linear[16];
	for (int i = 0; i < 16; i++) linear[i] = 0;
	int j = 0;
	for (int i = 0; i < 16; i++) {
		if (Ins[i]) linear[j]++;
		else j = i + 1;
	}
	string result = "";
	bool firstReg = true;
	result = "{";
	for (int i = 0; i < 16; i++) {
		if (linear[i]) {
			if (!firstReg) result += ", ";
			else firstReg = false;
			result += analyzeRegisterSet(linear, i);
		}
	}
	result += "}";
	return result;
}

//----------------------------------------
string getS(bool Ins[32]) {
	string result = "";
	if (Ins[20]) result = "S"; // if (S == 1)
	return result;
}

//----------------------------------------
string getFieldMask(bool Ins[32]) {
	string result = "_";
	if (Ins[16]) result += "c";
	if (Ins[17]) result += "x";
	if (Ins[18]) result += "s";
	if (Ins[19]) result += "f";

	if (result == "_") return ""; // none of those bits is 1
	return result;
}

//----------------------------------------
string getR(bool Ins[32]) {
	if (Ins[22]) return "SPSR";
	else return "CPSR";
}

//----------------------------------------
string getL(bool Ins[32]) {
	string result = "";
	if (Ins[24]) result = "L"; // if (L == 1)
	return result;
}

//----------------------------------------
string getSignedImmediate24(bool Ins[32], unsigned int InsCount) {
	unsigned int imm24 = BinToUnsignedInt(Ins, 23, 24);
	imm24 += InsCount; // InsCount is the address of the current instruction divided by 4
	if (imm24 < 8388608) imm24 = imm24 * 4 + 8; // 8388608 == 0x800000
	else imm24 = (imm24 - 16777215 + 1) * 4; // 16777215 == 0xFFFFFF
	string result = IntToHexString(imm24);
	return result;
}

//----------------------------------------
string getInsName(bool Ins[32]) {
	string result = "";

	if ((!Ins[27]) && (!Ins[26])) {
		if ((Ins[25]) || (!Ins[4]) || (!Ins[7])) { // Data-processing instructions
			int hash = BinToUnsignedInt(Ins, 24, 4); // get Opcode (bit[24-21])
		
			int SBO;
			int SBZ;
			int hash2;
			switch (hash) {
				case  0: result = "AND"; break;
				case  1: result = "EOR"; break;
				case  2: result = "SUB"; break;
				case  3: result = "RSB"; break;
				case  4: result = "ADD"; break;
				case  5: result = "ADC"; break;
				case  6: result = "SBC"; break;
				case  7: result = "RSC"; break;
				case  8: {
					if (Ins[20]) result = "TST";
					else {
						SBO = BinToUnsignedInt(Ins, 19, 4);
						SBZ = BinToUnsignedInt(Ins, 11, 12);
						if ((SBO == 15) && (SBZ == 0) && (!Ins[25])) result = "MRS";
						else result = __unDefIns;
					}
					break;
				}
				case  9: {
					if (Ins[20]) result = "TEQ"; // if (S == 1)
					else { // (S == 0)
						if (Ins[25]) { // if (I == 1)
							SBO = BinToUnsignedInt(Ins, 15, 4);
							if (SBO == 15) result = "MSR";
							else result = __unDefIns;
						}
						else { // (I == 0)
							hash2 = BinToUnsignedInt(Ins, 7, 4);
							if (hash2 == 0) {
								SBO = BinToUnsignedInt(Ins, 15, 4);
								SBZ = BinToUnsignedInt(Ins, 11, 4);
								if ((SBO == 15) && (SBZ == 0)) result = "MSR";
								else result = __unDefIns;
							}
							else if (hash2 == 1) {
								SBO = BinToUnsignedInt(Ins, 19, 12);
								if (SBO == 4095) result = "BX";
								else result = __unDefIns;
							}
							else result = __unDefIns;
						}
					}
					break;
				}
				case 10: {
					if (Ins[20]) {
						SBZ = BinToUnsignedInt(Ins, 15, 4);
						if (SBZ == 0) result = "CMP";
						else result = __unDefIns;
					}
					else {
						SBO = BinToUnsignedInt(Ins, 19, 4);
						SBZ = BinToUnsignedInt(Ins, 11, 12);
						if ((SBO == 15) && (SBZ == 0) && (!Ins[25])) result = "MRS";
						else result = __unDefIns;
					}
					break;
				}
				case 11: {
					if (Ins[20]) { // if (S == 1)
						SBZ = BinToUnsignedInt(Ins, 15, 4);
						if (SBZ == 0) result = "CMN";
						else result = __unDefIns;
					}
					else { // (S == 0)
						if (Ins[25]) { // if (I == 1)
							SBO = BinToUnsignedInt(Ins, 15, 4);
							if (SBO == 15) result = "MSR";
							else result = __unDefIns;
						}
						else { // (I == 0)
							hash2 = BinToUnsignedInt(Ins, 7, 4);
							SBO = BinToUnsignedInt(Ins, 15, 4);
							SBZ = BinToUnsignedInt(Ins, 11, 4);
							if ((hash2 == 0) && (SBO == 15) && (SBZ == 0)) result = "MSR";
							else result = __unDefIns;
						}
					}
					break;
				}
				case 12: result = "ORR"; break;
				case 13: {
					SBZ = BinToUnsignedInt(Ins, 19, 4);
					if (SBZ == 0) result = "MOV";
					else result = __unDefIns;
					break;
				}
				case 14: result = "BIC"; break;
				case 15: {
					SBZ = BinToUnsignedInt(Ins, 19, 4);
					if (SBZ == 0) result = "MVN";
					else result = __unDefIns;
					break;
				}
				default: break; // this line never runs
			}
			return result;
		}
	
		// (I == 0) && (bit 4 == 1) && (bit 7 == 1)
		int hash = BinToUnsignedInt(Ins, 24, 4);
		int hash2 = BinToUnsignedInt(Ins, 7, 4);
		int SBZ;
	
		if (hash2 == 9) { // bit[7-4] is 1001
			switch (hash) {
				case 0: {
					SBZ = BinToUnsignedInt(Ins, 15, 4);
					if (SBZ == 0) result = "MUL";
					else result = __unDefIns;
					break;
				}
				case 1: result = "MLA"; break;
				case 4: result = "UMULL"; break;
				case 5: result = "UMLAL"; break;
				case 6: result = "SMULL"; break;
				case 7: result = "SMLAL"; break;
				case 8: {
					SBZ = BinToUnsignedInt(Ins, 11, 4);
					if (SBZ == 0) result = "SWP";
					else result = __unDefIns;
					break;
				}
				case 10: {
					SBZ = BinToUnsignedInt(Ins, 11, 4);
					if (SBZ == 0) result = "SWPB";
					else result = __unDefIns;
					break;
				}
				default: result = __unDefIns; break;
			}
			return result;
		}
	
		if (hash2 == 11) { // bit[7-4] is 1011
			if (Ins[20]) result = "LDRH";
			else result = "STRH";
			return result;
		}
	
		if (hash2 == 13) { // bit[7-4] is 1101
			if (Ins[20]) result = "LDRSB";
			else result = __unDefIns;
			return result;
		}
	
		if (hash2 == 15) { // bit[7-4] is 1111
			if (Ins[20]) result = "LDRSH";
			else result = __unDefIns;
			return result;
		}
	
		return __unDefIns; // this line never runs
	}

	// (bit 27 == 1) || (bit 26 == 1)

	if ((!Ins[27]) && (Ins[26])) {
		if (Ins[20]) {
			if (Ins[22]) result = "LDRB";
			else result = "LDR";
		}
		else {
			if (Ins[22]) result = "STRB";
			else result = "STR";
		}
		return result;
	}

	if ((Ins[27]) && (!Ins[26])) {
		if (Ins[25]) result = "B";
		else {
			//if (Ins[22]) return = __unDefIns;
			if (Ins[20]) result = "LDM";
			else result = "STM";
		}
		return result;
	}

	if ((Ins[27]) && (Ins[26])) {
		if ((Ins[25]) && (Ins[24])) result = "SWI";
		else result = __unDefIns;
		return result;
	}

	return __unDefIns; // this line never runs
}

//----------------------------------------
void refineResult(string& result) {
	int found = result.rfind(__unDefIns);
	if (found != string::npos) result = __unDefIns;
	return;
}

//----------------------------------------
string disInstruction(unsigned long wordIns, unsigned int InsCount) {
	bool Ins[32];
	WordToBinary(wordIns, Ins);
	string InsName = getInsName(Ins);
	string result = __unDefIns;

	if ((InsName == "ADC") ||
		(InsName == "ADD") ||
		(InsName == "AND") ||
		(InsName == "BIC") ||
		(InsName == "EOR") ||
		(InsName == "ORR") ||
		(InsName == "RSB") ||
		(InsName == "RSC") ||
		(InsName == "SBC") ||
		(InsName == "SUB")) {
			result = InsName + getCondition(Ins) + getS(Ins) + "\t" + getRegister(Ins, 15) + ", " + getRegister(Ins, 19) + ", " + getShifterOperand(Ins);
	}

	if ((InsName == "CMN") ||
		(InsName == "CMP") ||
		(InsName == "TEQ") ||
		(InsName == "TST")) {
			result = InsName + getCondition(Ins) + "\t" + getRegister(Ins, 19) + ", " + getShifterOperand(Ins);
	}

	if ((InsName == "MOV") ||
		(InsName == "MVN")) {
			result = InsName + getCondition(Ins) + getS(Ins) + "\t" + getRegister(Ins, 15) + ", " + getShifterOperand(Ins);
	}

	if ((InsName ==  "LDR") ||
		(InsName == "LDRB") ||
		(InsName ==  "STR") ||
		(InsName == "STRB")) {
			result = InsName + getCondition(Ins) + "\t" + getRegister(Ins, 15) + ", " + getAddressingMode2(Ins);
	}

	if ((InsName ==  "LDRH") ||
		(InsName == "LDRSB") ||
		(InsName == "LDRSH") ||
		(InsName ==  "STRH")) {
			result = InsName + getCondition(Ins) + "\t" + getRegister(Ins, 15) + ", " + getAddressingMode3(Ins);
	}

	if ((InsName == "LDM") ||
		(InsName == "STM")) {
			result = InsName + getCondition(Ins) + getAddressingMode4(Ins) + "\t" + getRegister(Ins, 19) + getExclamation(Ins) + ", " + getRegisterList(Ins) + getCircumflex(Ins);
	}

	if ((InsName == "SMLAL") ||
		(InsName == "SMULL") ||
		(InsName == "UMLAL") ||
		(InsName == "UMULL")) {
			result = InsName + getCondition(Ins) + getS(Ins) + "\t" + getRegister(Ins, 15) + ", " + getRegister(Ins, 19) + ", " + getRegister(Ins, 3) +", " + getRegister(Ins, 11);
	}

	if ((InsName ==  "SWP") ||
		(InsName == "SWPB")) {
			result = InsName + getCondition(Ins) + "\t" + getRegister(Ins, 15) + ", " + getRegister(Ins, 3) +", [" + getRegister(Ins, 19) + "]";
	}

	if (InsName ==  "MLA") {
			result = InsName + getCondition(Ins) + getS(Ins) + "\t" + getRegister(Ins, 15) + ", " + getRegister(Ins, 3) + ", " + getRegister(Ins, 11) + ", " + getRegister(Ins, 19);
	}

	if (InsName == "MSR") {
		if (Ins[25]) result = InsName + getCondition(Ins) + "\t" + getR(Ins) + getFieldMask(Ins) + ", #" + IntToString(BinToUnsignedInt(Ins, 7, 8));
		else result = InsName + getCondition(Ins) + "\t" + getR(Ins) + getFieldMask(Ins) + ", " + getRegister(Ins, 3);
	}

	if (InsName == "B") {
			result = InsName + getL(Ins) + getCondition(Ins) + "\t" + getSignedImmediate24(Ins, InsCount);
	}

	if (InsName == "BX") {
			result = InsName + getCondition(Ins) + "\t" + getRegister(Ins, 3);
	}

	if (InsName == "MRS") {
			result = InsName + getCondition(Ins) + "\t" + getRegister(Ins, 15) + ", " + getR(Ins);
	}

	if (InsName == "MUL") {
			result = InsName + getCondition(Ins) + getS(Ins) + "\t" + getRegister(Ins, 19) + ", " + getRegister(Ins, 3) + ", " + getRegister(Ins, 11);
	}

	if (InsName == "SWI") {
			result = InsName + getCondition(Ins) + "\t" + IntToString(BinToUnsignedInt(Ins, 23, 24));
	}

	refineResult(result);
	return result;
}

//----------------------------------------
stringstream doDisassemble(CString filename) {
	CFile inputFile;
	stringstream outputstream;
	if (!inputFile.Open(filename, CFile::modeRead | CFile::typeBinary)) {
		outputstream << "error";
		return outputstream;
	}
	unsigned long wordIns;
	unsigned int InsCount = 0; // This variable is used to decode B/BL instruction
	while (inputFile.Read(&wordIns, 4)) {
		outputstream << disInstruction(wordIns, InsCount) << "\r\n";
		InsCount++;
	}
	inputFile.Close();
	return outputstream;
}

#endif
