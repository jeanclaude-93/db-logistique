/** ERP/SUPPLY LOG – règles de calcul (équivalent Java du moteur JavaScript : Stock, Flotte, Achats, Actifs, Maintenance). */
public final class CalculsLogistique {
    private CalculsLogistique() {}
    public enum StatutStock { CRITIQUE, REAPPRO, OK }

    // ---- Stock ----
    public static double stockMinimum(double cjm, double delaiJours, double stockSecurite) { return cjm * delaiJours + stockSecurite; }
    public static double stockActuel(double entrees, double sorties) { return entrees - sorties; }
    public static StatutStock statutStock(double stock, double stockSecurite, double stockMin) {
        return stock <= stockSecurite ? StatutStock.CRITIQUE : stock <= stockMin ? StatutStock.REAPPRO : StatutStock.OK;
    }
    // ---- Flotte & carburant ----
    public static double consommation100(double litres, double km) { return km > 0 ? litres / km * 100 : Double.NaN; }
    public static double normeZone(String zone) {
        String z = zone == null ? "" : zone.toLowerCase();
        return z.contains("asphalte") ? 12 : z.contains("masisi") ? 22 : z.contains("rutshuru") ? 16 : 15;
    }
    public static String alerteConso(double conso, double norme) {
        double e = (conso - norme) / norme;
        return Double.isNaN(e) ? "" : e > .15 ? "Surconsommation" : e < -.15 ? "Sous-consommation" : "Normal";
    }
    public static double amortissementKm(double km, double coutInitial, double residuelle, double dureeVieKm) {
        return km * (coutInitial - residuelle) / (dureeVieKm == 0 ? 1 : dureeVieKm);
    }
    // ---- Achats ----
    public static double budget(double qte, double puBudget) { return qte * puBudget; }
    public static double ecartAchat(double qte, double puReel, double puBudget) { return qte * (puReel - puBudget); }
    // ---- Actifs ----
    public static double valeurAchat(double qte, double pu) { return qte * pu; }
    public static double amortissementCumule(double qte, double pu, double tauxPct, int annees) {
        double v = valeurAchat(qte, pu);
        return Math.min(v, v * tauxPct / 100 * annees);
    }
    // ---- Maintenance ----
    public static double resteAvantService(double prochain, double compteur) { return prochain - compteur; }

    public static void main(String[] args) {
        System.out.println(stockMinimum(2, 5, 10) + " " + statutStock(8, 10, 20));
        System.out.printf("%.1f L/100 km -> %s%n", consommation100(22, 170), alerteConso(consommation100(22, 170), normeZone("Goma - Sake (Asphalte)")));
        System.out.println(amortissementCumule(5, 250, 20, 1));
    }
}
