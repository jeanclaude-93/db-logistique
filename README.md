# ERP/SUPPLY LOG — déploiement GitHub + Vercel

```
public/index.html   application (interface)
api/                fonctions serverless : auth.js, db.js, health.js (+ _lib.js)
moteur/             moteurs de calcul C++ et Java (identiques au JavaScript, avec tests)
tests/api.test.js   test de bout en bout de l'API   →  npm install && npm test
```

## Déployer en 6 étapes

1. **GitHub** : créez un dépôt (privé de préférence) et envoyez-y ce dossier.
   `git init && git add . && git commit -m "ERP/SUPPLY LOG" && git branch -M main && git remote add origin <URL> && git push -u origin main`
2. **Vercel** : *Add New → Project*, importez le dépôt, laissez tous les réglages par défaut (pas de build).
3. **Base de données** : dans le projet Vercel, onglet *Storage* → *Create / Marketplace* → **Neon (Postgres)** → connectez-le au projet.
   Vercel ajoute seul la variable `DATABASE_URL`. La table est créée automatiquement.
4. **Variables d'environnement** (*Settings → Environment Variables*) :
   - `SESSION_SECRET` : chaîne aléatoire d'au moins 32 caractères (`openssl rand -hex 32`)
   - `SETUP_KEY` : une clé de votre choix, utilisée une seule fois
5. **Redéployez** (*Deployments → Redeploy*) pour prendre en compte les variables.
6. **Première connexion** : ouvrez le site, connectez-vous avec le compte administrateur d'origine ; l'application demande
   la `SETUP_KEY` une seule fois et crée la base. Ensuite, chaque `git push` redéploie automatiquement.

## Fonctionnement et sécurité
- Les mots de passe sont hachés dans le navigateur (PBKDF2) : le mot de passe en clair n'est jamais envoyé.
- Le serveur délivre un jeton signé valable 12 h ; sans jeton, la base n'est pas lisible.
- Seul un super administrateur peut modifier les comptes et les rôles ; les autres utilisateurs ne reçoivent jamais les empreintes.
- Chaque enregistrement est versionné : si deux personnes modifient en même temps, la seconde est prévenue et la page se rafraîchit.
- Ouverte hors Vercel (fichier local), l'application retombe sur le stockage du navigateur, comme avant.

## Limites connues
- Pas de limitation du nombre d'essais de connexion (à ajouter avec Vercel Firewall ou Upstash Ratelimit si le site est public).
- Les droits par menu (lecture/saisie) sont appliqués dans l'interface ; côté serveur, tout utilisateur connecté peut enregistrer les données métier.
- Taille maximale de la base : environ 4 Mo (limite des fonctions Vercel).
