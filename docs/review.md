# Academic Advisor's Review

Translation of the original advisor's review ("Відгук керівника") of the qualification work's quality. The advisor's full name and the plagiarism-check document ID have been redacted for confidentiality; all percentages and numeric results are unchanged from the original.

> Review of the academic advisor on the quality of the qualification work of a student of Y. O. Paton Vocational College of Welding and Electronics.

The qualification work consists of an explanatory note of 65 sheets and contains 22 figures, 25 tables, and 13 references used.

The explanatory note contains an abstract, an introduction, general, special, operational, and economic parts, an occupational-safety section, conclusions, and a list of references.

The introduction reveals the relevance of the chosen topic, driven by the growing need to ensure control of air-environment parameters in residential and industrial premises in accordance with the requirements of DBN B.2.5-67:2013 and ISO 16000; it formulates the goal, object, subject, and methods of the study, and justifies the work's practical significance for ensuring safety and comfort.

In line with the assigned task, an architecture for a microclimate monitoring and control system based on the ESP32 microcontroller with FreeRTOS task distribution and wireless data transmission via Bluetooth SPP and MQTT is proposed. The system's purpose and application scope in residential apartments, offices, and small industrial facilities are examined, a comparative analysis against commercial alternatives (AirGradient ONE, Atmotube PRO 2, Airthings View Plus) is performed, and the operating principle is described at the structural-diagram level across four hierarchical tiers.

The special part justifies the selection of the component base for the sensor and actuator sections: the dual-core, FreeRTOS-capable ESP32 microcontroller; the BME280 sensor for measuring temperature, humidity, and atmospheric pressure; the MQ-2 gas analyzer for detecting combustible gases and smoke; the 16×2 LCD display; and a three-channel opto-isolated relay block providing galvanic isolation. The system's functional architecture, the distribution of FreeRTOS tasks between cores, the hysteresis and automatic-actuator-control algorithm are described, and the system's reliability (mean time between failures of 38,168 h), power consumption, and detailed calculations of power switching elements with RC snubber networks for suppressing switching interference are calculated.

The operational part describes the hardware and software methods used to monitor the system's operability. The use of test instruments (a multimeter, an oscilloscope, a logic analyzer) for diagnosing the sensors and relay block is examined, along with software tools — the Serial Monitor for debugging, LittleFS logging routines, and Bluetooth-based monitoring tools — which provide visibility into system state and timely detection of component faults.

The economic section of the qualification work presents a calculation of the costs of procuring electronic components, mounting materials and the enclosure, developer labor costs, and overhead production expenses, from which the production cost, contract price, and economic-efficiency indicators of the project are determined, including the annual economic effect, the efficiency coefficient, and a payback period of 3 months. A comparison of the proposed solution against purchasing ready-made commercial monitoring systems was carried out, which justified the economic feasibility of the in-house development.

The occupational-safety section examines the requirements for organizing safe working conditions during the system's development, installation, and operation: electrical safety using galvanic isolation via optocouplers and double insulation (Class II per DSTU EN 61140), safety during soldering work, protection from electromagnetic emissions and switching interference, fire safety when working with combustible gases, and workplace-organization requirements accounting for ergonomic norms and lighting.

During preparation of the qualification work, each section was reviewed and assessed by the relevant advisors according to established quality criteria. The assessment covered completeness of material presentation, structural logic, relevance to the topic, scientific level, practical significance, and formatting compliance with the requirements established by the college.

The general, special, and operational parts, the economic part, and the occupational-safety section were accepted by the respective advisors (consultants). Standards compliance review (normokontrol) has been passed. There are no objections to the content or formatting of the qualification work.

The qualification work was checked for plagiarism using the online service StrikePlagiarism.com (document ID: **[redacted for confidentiality — available on request]**). The check results confirmed the originality of the work; the text-uniqueness level exceeds 97%, meeting the established requirements. Extent of similarities found: similarity coefficient KP1 — 0.59%, citation coefficient — 0.00% (the verification certificate is attached).

In the course of completing the qualification work, the student, Damir, demonstrated deep theoretical knowledge and a high level of practical training in the specialty, showing confident command of the principles of designing ESP32-based microprocessor systems, selecting sensors and electronic components, and methods for calculating system reliability, power consumption, and power switching elements. During the work, the student showed initiative, responsibility, persistence, and consistency in achieving the set goals, as well as the ability to apply theoretical knowledge to solve specific engineering problems in the field of home and industrial automation.

The qualification work of Damir was completed at a high professional level and merits a grade of **"Excellent"**.

15.06.2026

Academic advisor, teacher, specialist — **[name redacted for confidentiality — available on request]**

---

*Footnote from the original form: "When reviewing the quality of a diploma project, alongside characterizing the quality of the graphical work, the coherence and literacy of the explanatory note, and the degree of independent work and initiative shown by the student, one should characterize the student's theoretical and practical training, demonstrating the ability to solve specific production and design problems based on the latest advances in engineering and technology. The overall project grade is given on a five-point scale."*
