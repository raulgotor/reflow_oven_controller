<!--
*** raulgotor, reflow_oven_controller, twitter_handle, Reflow Oven Controller, ESP32 based reflow oven controller with touch panel and reflow profile editor
-->



<!-- PROJECT SHIELDS -->
<!--
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

<!-- PROJECT LOGO -->
<br />
<p align="center">
  <a href="https://github.com/raulgotor/reflow_oven_controller">
    <!img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">Reflow Oven Controller</h3>

  <p align="center">
    ESP32 based reflow oven controller with touch panel and reflow profile editor
    <br />
    <a href="https://github.com/raulgotor/reflow_oven_controller"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/raulgotor/reflow_oven_controller">View Demo</a>
    ·
    <a href="https://github.com/raulgotor/reflow_oven_controller/issues">Report Bug</a>
    ·
    <a href="https://github.com/raulgotor/reflow_oven_controller/issues">Request Feature</a>
  </p>

<!-- TABLE OF CONTENTS -->
<details open="open">
  <summary><h2 style="display: inline-block">Table of Contents</h2></summary>
  <ol>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgements">Acknowledgements</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->

[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fraulgotor%2Freflow_oven_controller.svg?type=large)](https://app.fossa.com/projects/git%2Bgithub.com%2Fraulgotor%2Freflow_oven_controller?ref=badge_large)

## About The Project

[![Product Name Screen Shot][product-screenshot]](https://example.com)
[![FOSSA Status](https://app.fossa.com/api/projects/git%2Bgithub.com%2Fraulgotor%2Freflow_oven_controller.svg?type=shield)](https://app.fossa.com/projects/git%2Bgithub.com%2Fraulgotor%2Freflow_oven_controller?ref=badge_shield)

This is yet another Reflow oven controller. I wasn't fully convinced with what I
found around the different repositories, so I created my own.

Basic features are:

- Display (ILI9341 2.8" TFT) and touch panel (XPT2046) controlled by LVGL library v6.0
- Custom profile editor
- Watchdog timer supervised
- Running over FreeRTOS
- PID control (_coming soon_)
- Profile graphing (_coming soon_)

This firmware is meant to be run in the custom PCB which is shared in this repo (coming soon) or
any other compatible hardware following same schematics

### Built With

* ESP-IDF v4.4-dev-3042-g220590d599

<!-- GETTING STARTED -->
## Getting Started

### Installation

#### Prerequisites

- Install ESP-IDF Framework, follow the walk-through [here](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/linux-macos-setup.html).

#### Steps

1. Clone the repo
   ```sh
   git clone https://github.com/raulgotor/reflow_oven_controller.git
   ```
   
2. cd into the repo folder and build the firmware:
   ```sh
   cd reflow_oven_controller && idf.py build
   ```
   
3. Flash the firmware into the hardware:
   ```sh
   idf.py flash
   ```

<!-- USAGE EXAMPLES -->
## Usage

After booting, the main screen will be displayed. At the bottom left of this screen,
the current reflow profile is shown. Check that the selected profile is the right one.

Pressing __Start__ button initiate the whole reflow process.

If the process needs to be stopped, press the __Stop__ button.
The process may be resumed after the cooling phase has been completed.

For selecting / creating a different profile, check __Profile editor screen__ section.

### Main screen

This screen shows the state the device is at, the temperature, whether it is
heating or not and selected soldering profile. 

### Profile editor screen

This screen allows the user to create, edit and delete different soldering
profiles. 

The profiles can be tunned with the following parameters:

- Profile Name
- Preheating temperature
  - Target temperature (°C) to which the device will heat during the preheating phase
- Soaking time
  - Time (in seconds) that the soaking phase will last. During this phase, the
      temperature will be the same as in preheating phase
- Reflow temperature
   - Target temperature (°C) to which the device will heat during the reflow phase
- Dwell time
   - Time (in seconds) that the soaking phase will last. During this phase, the
     temperature will be same as reflow phase target temperature.

#### Selecting a profile

To select a profile, tap on the profiles drop down list and tap again on the desired profile
This profile will be used next time a reflow process starts.

#### Creating a profile

Tap on __New__ button. The profile editor will be shown. Provide a name, and tune
the different temperature and time parameters as needed.

#### Editing a profile

Select the profile you want to edit, and then press __Edit__ button. The profile editor
will shown. Tune the differents temperature and time parameters as needed.
   
### Further documentation


<!-- ROADMAP -->
## Roadmap

See the [open issues](https://github.com/raulgotor/reflow_oven_controller/issues) for a list of proposed features (and known issues).


<!-- CONTRIBUTING -->
## Contributing

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request



<!-- LICENSE -->
## License

Distributed under the MIT License. See `LICENSE` for more information.

<!-- CONTACT -->
## Contact

Raúl Gotor

Project Link: [https://github.com/raulgotor/reflow_oven_controller](https://github.com/raulgotor/reflow_oven_controller)


<!-- ACKNOWLEDGEMENTS -->
## Acknowledgements

* [LVGL - Light and Versatile Graphics Library](https://lvgl.io)
* [Embedded C Coding Standard, 2018 Michael Barr](https://barrgroup.com/sites/default/files/barr_c_coding_standard_2018.pdf)
* [Best README template](https://github.com/othneildrew/Best-README-Template)


<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/raulgotor/reflow_oven_controller.svg?style=for-the-badge
[contributors-url]: https://github.com/raulgotor/reflow_oven_controller/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/raulgotor/reflow_oven_controller.svg?style=for-the-badge
[forks-url]: https://github.com/raulgotor/reflow_oven_controller/network/members
[stars-shield]: https://img.shields.io/github/stars/raulgotor/reflow_oven_controller.svg?style=for-the-badge
[stars-url]: https://github.com/raulgotor/reflow_oven_controller/stargazers
[issues-shield]: https://img.shields.io/github/issues/raulgotor/reflow_oven_controller.svg?style=for-the-badge
[issues-url]: https://github.com/raulgotor/reflow_oven_controller/issues
[license-shield]: https://img.shields.io/github/license/raulgotor/reflow_oven_controller.svg?style=for-the-badge
[license-url]: https://github.com/raulgotor/reflow_oven_controller/blob/master/LICENSE.txt
[linkedin-shield]: https://img.shields.io/badge/-LinkedIn-black.svg?style=for-the-badge&logo=linkedin&colorB=555
[linkedin-url]: https://linkedin.com/in/raulgotor