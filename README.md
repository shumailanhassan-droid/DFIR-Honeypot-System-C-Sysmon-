# DFIR-Honeypot-System-C-Sysmon-
A defensive deception tool that creates **decoy files and registry artifacts** to detect unauthorized access and provide **real-time alerts with Sysmon attribution**.
#  DFIR Honeypot System (C++ + Sysmon)

A defensive deception tool that creates **decoy files and registry artifacts** to detect unauthorized access and provide **real-time alerts with Sysmon attribution**.

---

##  Features

-  **File Honeypot Monitoring**
  - Detects file creation, modification, and deletion
  - Targets sensitive decoy files:
    - `passwords.txt`
    - `bank_info.txt`
    - `employee_records.txt`

-  **Registry Honeypot**
  - Monitors:
    ```
    HKCU\Software\CorpSecurity\VPN_Backup
    ```
  - Detects unauthorized registry modifications

-  **Real-Time Alerts**
  - Console alerts
  - Popup alerts (Windows MessageBox)

- **Sysmon Integration**
  - Event ID 1 → Process creation (WHO did it)
  - Event ID 11 → File activity
  - Event ID 13 → Registry modification

-  **Logging**
  - All events saved to `log.txt`
  - Timestamped for forensic use

---

## 🧱 Architecture
      ┌──────────────────────┐
      │  Attacker Activity   │
      └─────────┬────────────┘
                │
    ┌───────────▼────────────┐
    │   Honeypot System      │
    │                        │
    │   File Monitor         │
    │   Registry Monitor     │
    └───────────┬────────────┘
                │
    ┌───────────▼────────────┐
    │   Sysmon Correlation   │
    └───────────┬────────────┘
                │
    ┌───────────▼────────────┐
    │ Alerts + Logging       │
    └────────────────────────┘
