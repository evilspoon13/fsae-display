import { Router } from "express";
import * as configController from "./config.controller";

const router = Router();

router.get("/", configController.getConfig);

router.get("/screens", configController.getScreenNames);

router.get("/screens/:screenId", configController.getScreenById);

router.delete("/screens/:screenId", configController.deleteScreenById);

router.post("/screens/:screenId", configController.updateConfig);

export default router;
