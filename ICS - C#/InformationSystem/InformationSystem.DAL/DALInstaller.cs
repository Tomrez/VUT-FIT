using InformationSystem.DAL;
using InformationSystem.DAL.Factories;
using InformationSystem.DAL.Mappers;
using InformationSystem.DAL.Migrator;
using InformationSystem.DAL.Options;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.DependencyInjection;

namespace InformationSystem.DAL;

public static class DALInstaller
{
    public static IServiceCollection AddDALServices(this IServiceCollection services, DALOptions options)
    {
        services.AddSingleton(options);

        if (options is null)
        {
            throw new InvalidOperationException("No persistence provider configured");
        }

        if (string.IsNullOrEmpty(options.DatabaseDirectory))
        {
            throw new InvalidOperationException($"{nameof(options.DatabaseDirectory)} is not set");
        }
        if (string.IsNullOrEmpty(options.DatabaseName))
        {
            throw new InvalidOperationException($"{nameof(options.DatabaseName)} is not set");
        }

        services.AddSingleton<IDbContextFactory<InformationSystemDbContext>>(_ =>
            new DbContextSqLiteFactory(options.DatabaseFilePath, options?.SeedDemoData ?? false));
        services.AddSingleton<IDbMigrator, DbMigrator>();

        services.AddSingleton<SubjectEntityMapper>();
        services.AddSingleton<ActivityEntityMapper>();
        services.AddSingleton<StudentEntityMapper>();
        services.AddSingleton<StudentInSubjectEntityMapper>();
        services.AddSingleton<StudentEvaluationEntityMapper>();
        return services;
    }
}
