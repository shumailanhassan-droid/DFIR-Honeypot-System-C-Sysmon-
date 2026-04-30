#  DFIR Honeypot System (C++ & Sysmon)

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

### 3. Compile the Program and Run the Program

- Build Solution and Run Final Project For Defense.exe

You should see:

- In Windows Console


<img width="392" height="177" alt="image" src="https://github.com/user-attachments/assets/1aa45f45-d2cc-4a1e-bbee-1b74ceb5bd35" />


---


- Decoy directory "Important" with decoy files in Users/Public/CorpData: 


<img width="937" height="358" alt="image" src="https://github.com/user-attachments/assets/a9038884-a8b6-4ff5-8911-b70d2a45dcfc" />





- log.txt (in the same folder as your .cpp)

---

##  Test the Honeypot 

### 1. File Attack Simulation
Open and modify a decoy file:
notepad DecoyFolder\passwords.txt

**Expected Results:** 

- Console alert/ Sysmon attribution (process info)

   <img width="1083" height="592" alt="image" src="https://github.com/user-attachments/assets/47f57532-8495-4bdc-bd77-65edc3c9d271" />


- Popup alert

  <img width="303" height="213" alt="image" src="https://github.com/user-attachments/assets/2a7bac53-8810-4684-816c-5f0d129fc725" />


### 2. Registry Attack Simulation
Run in Powershell (admin):
```
Set-ItemProperty -Path "HKCU:\Software\CorpSecurity\VPN_Backup" -Name "AdminBackupToken" -Value "ATTACK"
```

 **Expected Results:**

- Console alert/ Sysmon Event ID 13 (who modified it)

<img width="1026" height="310" alt="image" src="https://github.com/user-attachments/assets/26739281-cf36-4937-ab0e-522c1c19bcd4" />


- Popup alert

  <img width="308" height="193" alt="image" src="https://github.com/user-attachments/assets/045f422a-bacd-4572-99fc-f64ae51e60a8" />


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


