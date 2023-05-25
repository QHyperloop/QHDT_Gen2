# Web Based GUI

The web-based GUI has been updated over 2022-23 year to a JS, browser-based application.

There are two main components, a React frontend and Express backend.

## Start the App

First change directory to the **/web_based_gui/**. This is where all the GUI code is.

    cd web_base_gui

### Frontend

Change directory into the frontend directory.

    cd frontend

Install the dependencies for the frontend.

    npm i

Start the frontend

    npm start

### Backend

Change directory to backend directory

    cd ../backend

Install the dependencies for the backend

    npm i

Start the backend

    npm start

## Work to be completed

- Add UDP data decoding in **/backend/udp/udpConfig.js**

## Things to note

You will need node installed. Node version v18.13.0 was used for development.

The ports for UDP connections for the arduino can be configured in the **/backend/.env** file. The addresses and ports should be up-to-date with the arduino code.
