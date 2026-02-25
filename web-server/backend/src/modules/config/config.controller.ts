import type { Request, Response } from "express";
import type { GraphicsConfig, ScreenInfo } from "./config.types";
import * as configService from "./config.service";

export async function getConfig(_req: Request, res: Response) {
  const config: GraphicsConfig | null = await configService.readConfig();
  res.status(200).json(config);
}

export async function getScreenNames(req: Request, res: Response) {
  const screens: string[] | null = await configService.getScreenNames();
  if (screens === null) {
    res.status(404).json({msg: "No screens found"});
  }

  res.status(200).json({screens});
  return;
}

export async function getScreenById(req: Request<{ screenId: string }>, res: Response) {
  const screenName = req.params.screenId;
  if (!screenName) {
    res.status(400).json({ msg: "Invalid request" });
    return;
  }

  const screen: ScreenInfo | null = await configService.getScreenById(screenName);
  if (screen === null) {
    res.status(404).json({ msg: "Screen not found" });
    return;
  }

  res.status(200).json(screen);
  return;
}

export async function deleteScreenById(req: Request<{ screenId: string}>, res: Response) {
  const screenName = req.params.screenId;
  if (!screenName) {
    res.status(400).json({ msg: "Invalid request" });
    return;
  }

  const response = await configService.deleteScreenById(screenName);
  if (response.msg === "fail") {
    res.status(404).json({ success: false });
    return;
  }

  res.status(200).json({ success: true });
  return;
}

export async function updateConfig(req: Request<{ screenId: string}>, res: Response) {
  // const config: GraphicsConfig = req.body.config;
  const screenId = req.params.screenId;
  const screen = req.body as ScreenInfo;
  await configService.saveScreen(screenId, screen);
  res.status(200).json({ success: true });
  return;
}
