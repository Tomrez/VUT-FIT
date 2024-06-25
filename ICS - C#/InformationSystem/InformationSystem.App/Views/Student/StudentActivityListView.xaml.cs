using InformationSystem.App.ViewModels.Student;

namespace InformationSystem.App.Views.Student;

public partial class StudentActivityListView
{
	public StudentActivityListView(StudentActivityListViewModel viewModel)
        : base(viewModel)
	{
		InitializeComponent();
	}

    private void OnFrameTapped(object sender, EventArgs e)
    {
        var frame = (Frame)sender;
        var stackLayout = (VerticalStackLayout)frame.Content;
        var fullTextLabel = (Label)stackLayout.Children[1];
        var partTextLabel = (Label)stackLayout.Children[0];

        fullTextLabel.IsVisible = !fullTextLabel.IsVisible;
        partTextLabel.IsVisible = !partTextLabel.IsVisible;
    }
}