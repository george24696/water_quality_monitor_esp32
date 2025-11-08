# Local Firebase config (website_dashboard)

This file documents how to create a local `firebase-config.js` for the dashboard.
Do NOT commit that file to version control. The project `.gitignore` already
contains an entry for `website_dashboard/firebase-config.js`.

## Purpose

The dashboard expects a local file at `website_dashboard/firebase-config.js` that
exposes a global `window.appConfig` object with your Firebase configuration and
any local credentials the dashboard uses for automated sign-in (if applicable).

## Example template (DO NOT commit real keys)

Create a file named `website_dashboard/firebase-config.js` with content like:

```js
// firebase-config.js  (Local, private — DO NOT COMMIT)
// Replace placeholder values below with your Firebase project's values.
window.appConfig = {
  firebaseConfig: {
    apiKey: "REPLACE_WITH_API_KEY",
    authDomain: "REPLACE_WITH_PROJECT.firebaseapp.com",
    databaseURL: "https://REPLACE_WITH_PROJECT-default-rtdb.europe-west1.firebasedatabase.app/",
    projectId: "REPLACE_WITH_PROJECT_ID",
    storageBucket: "REPLACE_WITH_PROJECT.appspot.com",
    messagingSenderId: "REPLACE_WITH_SENDER_ID",
    appId: "REPLACE_WITH_APP_ID"
  },
  // Optional: dashboard uses a hard-coded email/password to sign in.
  // Consider using a safer auth flow before shipping to users.
  userEmail: "your-admin@example.com",
  userPassword: "a-strong-password"
};
```

Notes:
- Keep this file out of version control. The repository `.gitignore` should
  already include `website_dashboard/firebase-config.js` — verify before committing.
- If you accidentally commit secrets, rotate API keys and credentials immediately.

## Security recommendations

- Avoid embedding long-lived credentials in client-side JS for production.
- Restrict API key usage in the Google Cloud Console (HTTP referrers) and
  restrict database rules so unauthorized clients cannot read/write sensitive data.
- Prefer server-side authentication or custom token generation for admin actions.
- Remove `userEmail`/`userPassword` from the client and implement a safer login
  flow (OAuth, temporary tokens, or server-side proxy) if possible.

## Verifying it works

1. Create `website_dashboard/firebase-config.js` using the template above.
2. Open `website_dashboard/index.html` in a browser (or serve it with a local
   static server). The dashboard will check for `window.appConfig` and show
   `Config Error` in the UI if the file is missing or incorrectly formatted.

If you want, I can add a simple script to generate a template file automatically
or replace the email/password sign-in with a safer flow — tell me which you
prefer.
