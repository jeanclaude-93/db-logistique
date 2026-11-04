// ERP/SUPPLY LOG – règles de calcul (équivalent C++17 du moteur JavaScript).
#include <algorithm>
#include <cctype>
#include <cmath>
#include <iostream>
#include <string>

namespace logistique {

enum class StatutStock { Critique, Reappro, OK };

// ---- Stock ----
double stockMinimum(double cjm, double delaiJours, double stockSecurite) { return cjm * delaiJours + stockSecurite; }
double stockActuel(double entrees, double sorties) { return entrees - sorties; }
StatutStock statutStock(double stock, double stockSecurite, double stockMin) {
    return stock <= stockSecurite ? StatutStock::Critique : stock <= stockMin ? StatutStock::Reappro : StatutStock::OK;
}

// ---- Flotte & carburant ----
double consommation100(double litres, double km) { return km > 0 ? litres / km * 100 : std::nan(""); }
double normeZone(std::string zone) {
    std::transform(zone.begin(), zone.end(), zone.begin(), [](unsigned char c) { return std::tolower(c); });
    auto a = [&](const char* m) { return zone.find(m) != std::string::npos; };
    return a("asphalte") ? 12 : a("masisi") ? 22 : a("rutshuru") ? 16 : 15;
}
std::string alerteConso(double conso, double norme) {
    double e = (conso - norme) / norme;
    if (std::isnan(e)) return "";
    return e > .15 ? "Surconsommation" : e < -.15 ? "Sous-consommation" : "Normal";
}
double amortissementKm(double km, double coutInitial, double residuelle, double dureeVieKm) {
    return km * (coutInitial - residuelle) / (dureeVieKm == 0 ? 1 : dureeVieKm);
}

// ---- Achats ----
double budget(double qte, double puBudget) { return qte * puBudget; }
double ecartAchat(double qte, double puReel, double puBudget) { return qte * (puReel - puBudget); }

// ---- Actifs ----
double valeurAchat(double qte, double pu) { return qte * pu; }
double amortissementCumule(double qte, double pu, double tauxPct, int annees) {
    double v = valeurAchat(qte, pu);
    return std::min(v, v * tauxPct / 100 * annees);
}

// ---- Maintenance ----
double resteAvantService(double prochain, double compteur) { return prochain - compteur; }

}  // namespace logistique

int main() {
    using namespace logistique;
    double c = consommation100(22, 170);
    std::cout << stockMinimum(2, 5, 10) << " | " << c << " L/100 km -> " << alerteConso(c, normeZone("Goma - Sake (Asphalte)"))
              << " | " << amortissementCumule(5, 250, 20, 1) << "\n";
}
