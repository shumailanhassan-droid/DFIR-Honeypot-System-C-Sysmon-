#  DFIR Honeypot System (C++ + Sysmon)

A defensive deception tool that creates **decoy files and registry artifacts** to detect unauthorized access and provide **real-time alerts with Sysmon attribution**.

---

##  Features

  **File Honeypot Monitoring**
  - Detects file creation, modification, and deletion
  - Targets sensitive decoy files:
     `passwords.txt`
     `bank_info.txt`
     `employee_records.txt`

  **Registry Honeypot**
  - Monitors:
    ```
    HKCU\Software\CorpSecurity\VPN_Backup
    ```
  - Detects unauthorized registry modifications

  **Real-Time Alerts**
  - Console alerts
  - Popup alerts (Windows MessageBox)

 **Sysmon Integration**
  - Event ID 1 → Process creation (WHO did it)
  - Event ID 11 → File activity
  - Event ID 13 → Registry modification

  **Logging**
  - All events saved to `log.txt`
  - Timestamped for forensic use

---

##  Architecture
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


---

##  Setup Instructions

### 1. Install Sysmon
Download and install Sysmon from Microsoft: 

```
sysmon -i config.xml
```
---

### 2. Sysmon Configuration

Use this configuration:

```xml
<Sysmon schemaversion="4.90">
  <EventFiltering>

    <ProcessCreate onmatch="include" />

    <FileCreate onmatch="include">
      <TargetFilename condition="contains">DecoyFolder</TargetFilename>
    </FileCreate>

    <RegistryEvent onmatch="include">
      <TargetObject condition="contains">CorpSecurity</TargetObject>
    </RegistryEvent>

  </EventFiltering>
</Sysmon>

```
**Then reload the config:** 

```
sysmon -c config.xml
```
---

### 3. Compile the Program 
---

### 4. Run the Program

You should see:

DFIR Honeypot Active - Monitoring...

Registry monitoring active...

---

##  Test the Honeypot

### 1. File Attack Simulation
Open and modify a decoy file:
notepad DecoyFolder\passwords.txt

**Expected Results:** 

- Console alert

- Popup alert

- Sysmon attribution (process info)

### 2. Registry Attack Simulation
Run in Powershell (admin):
```
Set-ItemProperty -Path "HKCU:\Software\CorpSecurity\VPN_Backup" -Name "AdminBackupToken" -Value "ATTACK"
```

 **Expected Results:**

- Registry alert

- Sysmon Event ID 13 (who modified it)

### 3. Remote Attack (Kali Linus)

From Kali via SSH:

```
echo "hacked" >> Locatoin of the program/DecoyFolder/passwords.txt
```

**Example Output:**

[ALERT] FILE DECOY TRIGGERED | MODIFIED | File: passwords.txt | Process: Notepad.exe

Sysmon Attribution:

Process Create:

Image: powershell.exe

User: XXXXXX


[ALERT] REGISTRY HONEYPOT TRIGGERED!

Key: HKCU\Software\CorpSecurity\VPN_Backup

Sysmon Details:

EventType: SetValue

Image: powershell.exe

User: XXXXX

---

##  What This Project Demonstrates 

- Digital Forensics & Incident Response (DFIR)

- Windows API (C++)

- Real-time system monitoring

- Sysmon log correlation

- Threat detection via deception techniques

---

##  Limitations
- File read-only access detection is limited

- Sysmon delay may cause slight lag in attribution

- Requires proper Sysmon configuration

##  Author
Shumaila Hassan

-Digital Forensics Student

##  Screenshots
