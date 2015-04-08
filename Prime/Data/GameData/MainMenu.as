UIElement@ buttonContainer;
Button@ startButton;
Button@ multiplayerButton;
Button@ characterButton;
Button@ settingsButton;
Button@ exitButton;

void Start()
{
    XMLFile@ uiStyle = cache.GetResource("XMLFile", "UI/DefaultStyle.xml");
    // Set style to the UI root so that elements will inherit it
    ui.root.defaultStyle = uiStyle;

    buttonContainer = ui.root.CreateChild("UIElement");
    buttonContainer.SetFixedSize(graphics.width, graphics.height);
    buttonContainer.SetPosition(0, 0);
    buttonContainer.layoutMode = LM_VERTICAL;

    startButton = CreateButton("Start");
    multiplayerButton = CreateButton("Multiplayer");
    characterButton = CreateButton("Character");
    settingsButton = CreateButton("Settings");
    exitButton = CreateButton("Exit");

    SubscribeToEvent(startButton, "Released", "HandleStart");
    SubscribeToEvent(multiplayerButton, "Released", "HandleMultiplayer");
    SubscribeToEvent(characterButton, "Released", "HandleCharacter");
    SubscribeToEvent(settingsButton, "Released", "HandleSettings");
    SubscribeToEvent(exitButton, "Released", "HandleExit");
}

Button@ CreateButton(const String& text)
{
    Font@ font = cache.GetResource("Font", "Fonts/Anonymous Pro.ttf");
    
    Button@ button = buttonContainer.CreateChild("Button");
    button.SetStyleAuto();
    button.SetFixedWidth(240);
    
    Text@ buttonText = button.CreateChild("Text");
    buttonText.SetFont(font, 24);
    buttonText.SetAlignment(HA_CENTER, VA_CENTER);
    buttonText.text = text;
    
    return button;
}

void HandleStart(StringHash eventType, VariantMap& eventData)
{   
    core.LoadMap("testlevel");
}

void HandleMultiplayer(StringHash eventType, VariantMap& eventData)
{   
    
}

void HandleCharacter(StringHash eventType, VariantMap& eventData)
{   

}

void HandleSettings(StringHash eventType, VariantMap& eventData)
{   

}

void HandleExit(StringHash eventType, VariantMap& eventData)
{   
    engine.Exit();
}