// ERP/SUPPLY LOG – moteur de calcul (C++17), équivalent du moteur JavaScript + indicateurs avancés.
// Compilation : g++ -std=c++17 -O2 -Wall calculs_logistique.cpp -o logistique && ./logistique
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <iostream>
#include <limits>
#include <numeric>
#include <string>
#include <vector>

namespace logistique {

enum class StatutStock { Critique, Reappro, OK };
constexpr double NAN_D = std::numeric_limits<double>::quiet_NaN();

// ---- Stock ----
double stockMinimum(double cjm, double delaiJours, double stockSecurite) { return cjm * delaiJours + stockSecurite; }
double stockActuel(double entrees, double sorties) { return entrees - sorties; }
StatutStock statutStock(double stock, double stockSecurite, double stockMin) {
    return stock <= stockSecurite ? StatutStock::Critique : stock <= stockMin ? StatutStock::Reappro : StatutStock::OK;
}
const char* libelle(StatutStock s) { return s == StatutStock::Critique ? "Critique" : s == StatutStock::Reappro ? "Réappro" : "OK"; }
// Nouveaux indicateurs
double couvertureJours(double stock, double cjm) { return cjm > 0 ? stock / cjm : NAN_D; }
double tauxRotation(double sortiesPeriode, double stockMoyen) { return stockMoyen > 0 ? sortiesPeriode / stockMoyen : NAN_D; }
double quantiteEconomique(double demandeAnnuelle, double coutCommande, double coutPossessionUnitaire) {  // formule de Wilson
    return (demandeAnnuelle > 0 && coutCommande > 0 && coutPossessionUnitaire > 0)
               ? std::sqrt(2 * demandeAnnuelle * coutCommande / coutPossessionUnitaire) : NAN_D;
}
// Classification ABC (Pareto) : renvoie 'A' (80 % de la valeur), 'B' (15 %), 'C' (5 %) pour chaque article.
std::vector<char> classeABC(const std::vector<double>& valeurs) {
    std::vector<size_t> idx(valeurs.size());
    std::iota(idx.begin(), idx.end(), 0);
    std::sort(idx.begin(), idx.end(), [&](size_t a, size_t b) { return valeurs[a] > valeurs[b]; });
    double total = std::accumulate(valeurs.begin(), valeurs.end(), 0.0), cumul = 0;
    std::vector<char> res(valeurs.size(), 'C');
    for (size_t i : idx) {
        cumul += valeurs[i];
        double part = total > 0 ? cumul / total : 1;
        res[i] = part <= .80 + 1e-9 ? 'A' : part <= .95 + 1e-9 ? 'B' : 'C';
    }
    if (!idx.empty() && total > 0) res[idx[0]] = 'A';  // le premier article est toujours en A
    return res;
}

// ---- Flotte & carburant ----
double consommation100(double litres, double km) { return km > 0 ? litres / km * 100 : NAN_D; }
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
double coutParKm(double carburant, double maintenance, double amort, double km) { return km > 0 ? (carburant + maintenance + amort) / km : NAN_D; }

// ---- Achats ----
double budget(double qte, double puBudget) { return qte * puBudget; }
double ecartAchat(double qte, double puReel, double puBudget) { return qte * (puReel - puBudget); }

// ---- Actifs ----
double valeurAchat(double qte, double pu) { return qte * pu; }
double amortissementCumule(double qte, double pu, double tauxPct, int annees) {
    double v = valeurAchat(qte, pu);
    return std::min(v, v * tauxPct / 100 * annees);
}
double valeurNetteComptable(double qte, double pu, double tauxPct, int annees) {
    return valeurAchat(qte, pu) - amortissementCumule(qte, pu, tauxPct, annees);
}

// ---- Maintenance ----
double resteAvantService(double prochain, double compteur) { return prochain - compteur; }
bool serviceEnRetard(double prochain, double compteur) { return resteAvantService(prochain, compteur) < 0; }

}  // namespace logistique

static bool proche(double a, double b) { return std::fabs(a - b) < 1e-6; }

int main() {
    using namespace logistique;
    // Tests d’auto-vérification (cohérents avec le moteur JavaScript)
    assert(proche(stockMinimum(2, 5, 10), 20));
    assert(statutStock(8, 10, 20) == StatutStock::Critique && statutStock(15, 10, 20) == StatutStock::Reappro && statutStock(25, 10, 20) == StatutStock::OK);
    assert(alerteConso(consommation100(22, 170), normeZone("Goma - Sake (Asphalte)")) == "Normal" && alerteConso(25, 12) == "Surconsommation");
    assert(proche(amortissementCumule(5, 250, 20, 1), 250) && proche(amortissementCumule(5, 250, 20, 9), 1250));
    assert(proche(quantiteEconomique(1000, 50, 2), 223.606797749979));
    assert((classeABC({800, 120, 50, 20, 10}) == std::vector<char>{'A', 'B', 'C', 'C', 'C'}));
    assert(serviceEnRetard(10000, 10250) && std::isnan(consommation100(5, 0)));
    std::cout << "Tous les tests passent.\n";
    double c = consommation100(22, 170);
    std::cout << "Stock min : " << stockMinimum(2, 5, 10) << " | Conso : " << c << " L/100 km (" << alerteConso(c, normeZone("Masisi")) << ")"
              << " | VNC : " << valeurNetteComptable(5, 250, 20, 1) << "\n";
}
