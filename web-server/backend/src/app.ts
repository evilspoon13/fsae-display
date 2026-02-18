import express from "express";
import configRoutes from "./modules/config/config.routes";
import dbcRotues from "./modules/dbc/dbc.routes";

const app = express();

app.use(express.json());

app.get("/", (_req, res) => {
  res.send("FSAE Driver Display - Web Server");
});

app.use("/api/config", configRoutes);
app.use("/api/dbc", dbcRotues);

export default app;
