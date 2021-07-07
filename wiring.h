#ifndef WIRING_H
#define WIRING_H

void TestWireInit(void);

void LoseWireInit(void);
void SetLoseOutput(int outVal);
int GetLoseInput(void);

void InputTransmitWireInit(enum PlayerIDs PlayerID);
int ReadInputWire(void);

void ModeWireInit(enum PlayerIDs PlayerID);
void SetModeOutput(int outVal);
int GetModeInput(void);

void AcknowledgeWireInit(void);
void ToggleAcknowledgeWireOutput(void);
void SetAcknowledgeWireOutput(int outVal);
int GetAcknowledgeWireInput(void);

void DependentWireSetup(enum PlayerIDs PlayerID);

#endif /* WIRING_H */