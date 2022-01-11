﻿using System.Windows;

namespace Editor.GameProject;

public partial class ProjectBrowserDialog : Window
{
    public ProjectBrowserDialog()
    {
        InitializeComponent();
    }

    private void OpenProjectButton_OnClick(object sender, RoutedEventArgs e)
    {
        if (sender == openProjectButton)
        {
            if (createProjectButton.IsChecked == true)
            {
                createProjectButton.IsChecked = false;
                bowserContent.Margin = new Thickness(0);
            }
            
            openProjectButton.IsChecked = true;
        }
        else
        {
            if (openProjectButton.IsChecked == true)
            {
                openProjectButton.IsChecked = false;
                bowserContent.Margin = new Thickness(-800, 0, 0, 0);
            }
            
            createProjectButton.IsChecked = true;
        }
    }
}