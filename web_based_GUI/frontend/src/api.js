import axios from "axios";

const api = axios.create({
  baseURL: "http://localhost:5000",
});

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

export { emergencyStop, safe, ready };
