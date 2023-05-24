import axios from "axios";

const api = axios.create({
  baseURL: `${process.env.REACT_APP_API_URL || "http://localhost"}:${
    process.env.REACT_APP_API_PORT || "5000"
  }`,
});

async function initialize() {
  try {
    const response = await api.post("/initialize");
    console.log(response);
  } catch (error) {
    console.error(error);
  }
}

async function emergencyStop() {
  try {
    const response = await api.post("/emergencyStop");
    console.log(response);
  } catch (error) {
    console.error(error);
  }
}

async function safe() {
  try {
    const response = await api.post("/safe");
    console.log(response);
  } catch (error) {
    console.error(error);
  }
}

async function ready() {
  try {
    const response = await api.post("/ready");
    console.log(response);
  } catch (error) {
    console.error(error);
  }
}

async function getSensors() {
  try {
    const response = await api.get("/sensors");
    console.log(response);
    return response.data;
  } catch (error) {
    console.error(error);
  }
}

export { initialize, emergencyStop, safe, ready, getSensors };
