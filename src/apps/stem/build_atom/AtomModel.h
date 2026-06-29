#pragma once

class AtomModel {
public:
  void reset();

  bool addProton();
  bool addNeutron();
  bool addElectron();

  bool removeProton();
  bool removeNeutron();
  bool removeElectron();

  int getProtons() const;
  int getNeutrons() const;
  int getElectrons() const;
  int getAtomicNumber() const;
  int getMassNumber() const;
  int getCharge() const;

  const char* getElementSymbol() const;
  const char* getElementName() const;

  bool isNeutral() const;

private:
  static constexpr int MaxProtons = 10;
  static constexpr int MaxElectrons = 10;
  static constexpr int ShellOneCapacity = 2;
  static constexpr int ShellTwoCapacity = 8;
  static constexpr int MaxNeutrons = 16;

  int protons = 0;
  int neutrons = 0;
  int electrons = 0;

  static const char* symbolForProtons(int count);
  static const char* nameForProtons(int count);
};

