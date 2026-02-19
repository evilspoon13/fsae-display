import { Router } from "express";
import * as configController from "./config.controller";

const router = Router();

router.get("/", configController.getConfig);

router.post("/", configController.updateConfig);

router.post("/reset", configController.resetConfig);

export default router;
